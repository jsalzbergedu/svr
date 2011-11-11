
#include <svr.h>
#include <svr/server/svr.h>

#include <ctype.h>

#include "sources/sources.h"

static void SVRs_Source_addType(SVRs_SourceType* source_type);

static Dictionary* sources = NULL;
static Dictionary* source_types = NULL;
static pthread_mutex_t sources_lock = PTHREAD_MUTEX_INITIALIZER;

void SVRs_Source_init(void) {
    sources = Dictionary_new();
    source_types = Dictionary_new();

    SVRs_Source_addType(&SVR_SOURCE(test));
    SVRs_Source_addType(&SVR_SOURCE(cam));
    SVRs_Source_addType(&SVR_SOURCE(file));
}

SVRs_Source* SVRs_Source_getByName(const char* source_name) {
    return Dictionary_get(sources, source_name);
}

SVRs_Source* SVRs_Source_openInstance(const char* source_name, const char* descriptor_orig) {
    SVRs_Source* source;
    Dictionary* arguments;
    SVRs_SourceType* source_type;
    char* descriptor;
    char* name;
    char* arg_name;
    char* arg_value;
    int offset;
    int end;

    offset = 0;
    descriptor = strdup(descriptor_orig);

    while(isspace(descriptor[offset])) {
        offset++;
    }

    name = descriptor + offset;
    while(isalnum(descriptor[offset]) || descriptor[offset] == '_') {
        offset++;
    }
    end = offset;

    /* Consume whitespace */
    while(isspace(descriptor[offset])) {
        offset++;
    }

    if(descriptor[offset] != ':') {
        SVR_log(SVR_DEBUG, "Syntax error before ':'");
        free(descriptor);
        return NULL;
    }
    descriptor[end] = '\0';
    offset++;

    arguments = Dictionary_new();
    while(descriptor[offset] != '\0') {
        /* Consume whitespace */
        while(isspace(descriptor[offset])) {
            offset++;
        }

        arg_name = descriptor + offset;
        while(isalpha(descriptor[offset])) {
            offset++;
        }
        end = offset;

        /* Consume whitespace */
        while(isspace(descriptor[offset])) {
            offset++;
        }

        if(descriptor[offset] != '=') {
            SVR_log(SVR_DEBUG, Util_format("Expected '=' at %d", offset));
            Dictionary_destroy(arguments);
            free(descriptor);
            return NULL;
        }
        offset++;

        /* Terminate argument name */
        descriptor[end] = '\0';

        /* Consume whitespace */
        while(isspace(descriptor[offset])) {
            offset++;
        }

        arg_value = descriptor + offset;
        while(strchr(", \t", descriptor[offset]) == NULL) {
            offset++;
        }
        end = offset;

        /* Consume whitespace */
        while(isspace(descriptor[offset])) {
            offset++;
        }

        if(descriptor[offset] == '\0') {
            descriptor[end] = '\0';
            Dictionary_set(arguments, arg_name, arg_value);
            break;
        }

        if(descriptor[offset] != ',') {
            SVR_log(SVR_DEBUG, Util_format("Expected ',' at %d", offset));
            Dictionary_destroy(arguments);
            free(descriptor);
            return NULL;
        }

        descriptor[end] = '\0';
        Dictionary_set(arguments, arg_name, arg_value);
        offset++;
    }

    source_type = Dictionary_get(source_types, name);
    if(source_type == NULL) {
        SVR_log(SVR_DEBUG, Util_format("No such source type '%s'", name));
        Dictionary_destroy(arguments);
        free(descriptor);
        return NULL;
    }

    source = source_type->open(source_name, arguments);

    Dictionary_destroy(arguments);
    free(descriptor);

    return source;
}

void SVRs_Source_fromFile(const char* filename) {
    Dictionary* source_descriptions = Config_readFile(filename);
    List* source_names;
    char* source_name;

    if(source_descriptions == NULL) {
        switch(Config_getError()) {
        case CONFIG_EFILEACCESS:
            SVR_log(SVR_CRITICAL, Util_format("Failed to open source description file: %s", strerror(errno)));
            break;
        case CONFIG_ELINETOOLONG:
            SVR_log(SVR_CRITICAL, Util_format("Line exceeded maximum allowable length at line %d", Config_getLineNumber()));
            break;
        case CONFIG_EPARSE:
            SVR_log(SVR_CRITICAL, Util_format("Parse error occurred on line %d", Config_getLineNumber()));
            break;
        default:
            SVR_log(SVR_CRITICAL, "Unknown error occurred while reading source description file");
            break;
        }

        SVRs_exitError();
        return;
    }

    source_names = Dictionary_getKeys(source_descriptions);
    for(int i = 0; (source_name = List_get(source_names, i)) != NULL; i++) {
        if(SVRs_Source_openInstance(source_name, Dictionary_get(source_descriptions, source_name)) == NULL) {
            SVR_log(SVR_CRITICAL, "Error parsing stream descriptor and/or starting stream");
            SVRs_exitError();
        }
        SVR_log(SVR_INFO, Util_format("Opened source \"%s\"", source_name));
    }

    List_destroy(source_names);
    Dictionary_destroy(source_descriptions);
}

static void SVRs_Source_addType(SVRs_SourceType* source_type) {
    Dictionary_set(source_types, source_type->name, source_type);
}

SVRs_Source* SVRs_Source_new(const char* name) {
    SVRs_Source* source;

    pthread_mutex_lock(&sources_lock);
    if(Dictionary_exists(sources, name)) {
        pthread_mutex_unlock(&sources_lock);
        return NULL;
    }

    source = malloc(sizeof(SVRs_Source));
    source->name = strdup(name);
    source->streams = List_new();
    source->frame_properties = NULL;
    source->decoder = NULL;
    source->cleanup = NULL;
    source->private_data = NULL;
    SVR_LOCKABLE_INIT(source);

    Dictionary_set(sources, name, source);
    pthread_mutex_unlock(&sources_lock);

    return source;
}

void SVRs_Source_destroy(SVRs_Source* source) {
    pthread_mutex_lock(&sources_lock);
    if(Dictionary_exists(sources, source->name) == false) {
        pthread_mutex_unlock(&sources_lock);
        return;
    }

    Dictionary_remove(sources, source->name);
    pthread_mutex_unlock(&sources_lock);

    SVR_LOCK(source);
    if(source->cleanup) {
        source->cleanup(source);
    }

    /* Send signals to streams first? */
    List_destroy(source->streams);

    if(source->frame_properties) {
        SVR_FrameProperties_destroy(source->frame_properties);
    }

    free(source->name);
    SVR_Decoder_destroy(source->decoder);
    SVR_UNLOCK(source);

    free(source);
}

int SVRs_Source_setEncoding(SVRs_Source* source, SVR_Encoding* encoding) {
    if(source->decoder) {
        /* Source already started */
        return SVR_INVALIDSTATE;
    }

    source->encoding = encoding;
    return SVR_SUCCESS;
}

int SVRs_Source_setFrameProperties(SVRs_Source* source, SVR_FrameProperties* frame_properties) {
    if(source->decoder) {
        /* Source already started */
        return SVR_INVALIDSTATE;
    }

    if(source->frame_properties) {
        SVR_FrameProperties_destroy(source->frame_properties);
    }

    source->frame_properties = SVR_FrameProperties_clone(frame_properties);
    return SVR_SUCCESS;
}

SVR_FrameProperties* SVRs_Source_getFrameProperties(SVRs_Source* source) {
    return source->frame_properties;
}

void SVRs_Source_registerStream(SVRs_Source* source, SVRs_Stream* stream) {
    SVR_LOCK(source);
    List_append(source->streams, stream);
    SVR_UNLOCK(source);
}

void SVRs_Source_unregisterStream(SVRs_Source* source, SVRs_Stream* stream) {
    SVR_LOCK(source);
    List_remove(source->streams, List_indexOf(source->streams, stream));
    SVR_UNLOCK(source);
}

int SVRs_Source_provideData(SVRs_Source* source, void* data, size_t data_available) {
    SVRs_Stream* stream;
    IplImage* frame;

    SVR_LOCK(source);
    if(source->decoder == NULL) {
        if(source->encoding == NULL || source->frame_properties == NULL) {
            SVR_UNLOCK(source);
            return SVR_INVALIDSTATE;
        }

        source->decoder = SVR_Decoder_new(source->encoding, source->frame_properties);
    }

    SVR_Decoder_decode(source->decoder, data, data_available);
    while(SVR_Decoder_framesReady(source->decoder) > 0) {
        frame = SVR_Decoder_getFrame(source->decoder);
        for(int i = 0; (stream = List_get(source->streams, i)) != NULL; i++) {
            SVRs_Stream_inputSourceFrame(stream, frame);
        }
        SVR_Decoder_returnFrame(source->decoder, frame);
    }
    SVR_UNLOCK(source);

    return SVR_SUCCESS;
}
