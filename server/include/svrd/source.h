
#ifndef __SVR_SERVER_SOURCE_H
#define __SVR_SERVER_SOURCE_H

#include <svr/forward.h>

struct SVRD_SourceFrame_s {
    IplImage* frame;
    SVRD_Source* source;
    SVR_REFCOUNTED;
};

struct SVRD_Source_s {
    char* name;

    SVR_Encoding* encoding;
    Dictionary* encoding_options;
    SVR_Decoder* decoder;
    SVR_FrameProperties* frame_properties;

    SVRD_SourceFrame* current_frame;
    pthread_mutex_t current_frame_lock;
    pthread_cond_t new_frame;

    SVRD_SourceType* type;
    void* private_data;

    bool closed;

    SVR_LOCKABLE;
    SVR_REFCOUNTED;
};

struct SVRD_SourceType_s {
    const char* name;
    SVRD_Source* (*open)(const char* name, Dictionary* arguments);
    void (*close)(SVRD_Source* source);
};

#define SVR_SOURCE(name) __svr_##name##_source

void SVRD_Source_init(void);
bool SVRD_Source_exists(const char* source_name);
SVRD_Source* SVRD_Source_getByName(const char* source_name);
SVRD_Source* SVRD_Source_getLockedSource(const char* source_name);
List* SVRD_Source_getSourcesList(void);
SVRD_Source* SVRD_Source_openInstance(const char* source_name, const char* descriptor_orig, int* return_code);
void SVRD_Source_fromFile(const char* filename);

SVRD_Source* SVRD_Source_new(const char* name);
void SVRD_Source_destroy(SVRD_Source* source);
SVR_FrameProperties* SVRD_Source_getFrameProperties(SVRD_Source* source);
int SVRD_Source_setEncoding(SVRD_Source* source, const char* encoding_descriptor);
int SVRD_Source_setFrameProperties(SVRD_Source* source, SVR_FrameProperties* frame_properties);
void SVRD_Source_adjustStreamPriority(SVRD_Source* source, SVRD_Stream* stream);
void SVRD_Source_dismissPausedStreams(SVRD_Source* source);
SVRD_SourceFrame* SVRD_Source_getFrame(SVRD_Source* source, SVRD_Stream* stream, SVRD_SourceFrame* last_frame);
int SVRD_Source_provideData(SVRD_Source* source, void* data, size_t data_available);

#endif // #ifndef __SVR_SERVER_SOURCE_H




