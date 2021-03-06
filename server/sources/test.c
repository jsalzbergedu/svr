
#include "svr.h"
#include "svrd.h"

static SVRD_Source* TestSource_open(const char* name, Dictionary* arguments);
static void TestSource_close(SVRD_Source* source);

SVRD_SourceType SVR_SOURCE(test) = {
        .name = "test",
        .open = TestSource_open,
        .close = TestSource_close
};

typedef struct {
    int width;
    int height;
    bool grayscale;
    int rate;
    pthread_t thread;
    bool close;
} SVRD_TestSource;

static void* TestSource_background(void* _source);

static SVRD_Source* TestSource_open(const char* name, Dictionary* arguments) {
    SVRD_TestSource* source_data = malloc(sizeof(SVRD_TestSource));
    SVR_FrameProperties* frame_properties;
    SVRD_Source* source;
    char* arg;

    source_data->width = 640;
    source_data->height = 480;
    source_data->grayscale = false;
    source_data->rate = 10;
    source_data->close = false;

    if(Dictionary_exists(arguments, "width")) {
        source_data->width = atoi(Dictionary_get(arguments, "width"));
    }

    if(Dictionary_exists(arguments, "height")) {
        source_data->height = atoi(Dictionary_get(arguments, "height"));
    }

    if(Dictionary_exists(arguments, "grayscale")) {
        arg = Dictionary_get(arguments, "grayscale");
        if(strcmp(arg, "1") == 0 || strcmp(arg, "true") == 0) {
            source_data->grayscale = true;
        } else if(strcmp(arg, "0") == 0 || strcmp(arg, "false") == 0) {
            source_data->grayscale = false;
        } else {
            SVR_log(SVR_ERROR, "Invalid value for grayscale in test source");
            free(source_data);
            return NULL;
        }
    }

    if(Dictionary_exists(arguments, "rate")) {
        source_data->rate = atoi(Dictionary_get(arguments, "rate"));
    }

    frame_properties = SVR_FrameProperties_new();
    frame_properties->width = source_data->width;
    frame_properties->height = source_data->height;
    frame_properties->channels = source_data->grayscale ? 1 : 3;
    frame_properties->depth = 8;

    source = SVRD_Source_new(name);
    SVRD_Source_setEncoding(source, "raw");
    SVRD_Source_setFrameProperties(source, frame_properties);
    SVR_FrameProperties_destroy(frame_properties);

    if(source == NULL) {
        SVR_log(SVR_ERROR, Util_format("Error creating source '%s'", name));
        free(source_data);
        return NULL;
    }

    source->private_data = source_data;

    pthread_create(&source_data->thread, NULL, TestSource_background, source);

    return source;
}

static void* TestSource_background(void* _source) {
    SVRD_Source* source = (SVRD_Source*) _source;
    SVRD_TestSource* source_data = (SVRD_TestSource*) source->private_data;
    IplImage* frame;
    int width = source_data->width;
    int height = source_data->height;
    int block_x = 0;
    int block_y = 0;
    float sleep = 0;
    CvScalar colors[] = {CV_RGB(255, 0, 0),
                         CV_RGB(0, 255, 0),
                         CV_RGB(0, 0, 255)
    };

    frame = SVR_FrameProperties_imageFromProperties(SVRD_Source_getFrameProperties(source));

    if(source_data->rate > 0) {
        sleep = 1.0 / source_data->rate;
    }

    while(source_data->close == false) {
        /* Generate image with a colored rectangle that moves */
        cvSetImageROI(frame, cvRect(0, 0, width, height));
        cvSet(frame, CV_RGB(0, 0, 0), NULL);
        cvSetImageROI(frame, cvRect(block_x, block_y, 64, 48));
        cvSet(frame, colors[rand() % 3], NULL);

        /* Move to next block */
        block_x = (block_x + 64) % width;
        if(block_x == 0) {
            block_y = (block_y + 48) % height;
        }

        SVRD_Source_provideData(source, (void*) frame->imageData, frame->imageSize);
        Util_usleep(sleep);
    }

    cvReleaseImage(&frame);

    return NULL;
}

static void TestSource_close(SVRD_Source* source) {
    SVRD_TestSource* source_data = (SVRD_TestSource*) source->private_data;

    source_data->close = true;
    pthread_join(source_data->thread, NULL);
    free(source_data);
}
