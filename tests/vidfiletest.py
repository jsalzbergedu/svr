'''Show all SVR streams, and show any new streams that appear.'''

from __future__ import division

import time

import cv

import svr

import cv2

import numpy as np

import os

from threading import Thread

svr.connect()

open_streams = {} # Map stream names to stream objects
last_sources_update = 0  # Last time that the open_streams was updated
SOURCES_UPDATE_FREQUENCY = 5  # How many times per second to update sources

record = 1

def cv_to_cv2(frame):
    '''Convert a cv image into cv2 format.

    Keyword Arguments:
    frame -- a cv image
    Returns a numpy array that can be used by cv2.
    '''
    cv2_image = np.asarray(frame[:, :])
    return cv2_image

def getargs():
    return ('testfile.avi', 'forward')

def record_frame(stream_name, stream, out, filename):
    frame = None
    try:
        frame = stream.get_frame()
    except svr.OrphanStreamExeption:
        # Closed Stream
        del open_streams[stream_name]
        print "Stream Closed:", stream_name
        
    if frame:
        if out.isOpened():
            raw_frame = cv_to_cv2(frame)
            out.write(raw_frame)
        else: # Closes out.isOpened()
            cv2.imwrite("{}{}{}.jpeg".format(stream_name,filename, i), raw_frame)
            i += 1
            
if __name__ == "__main__":
    (filename, stream_arg) = getargs()
    fourcc = cv2.cv.CV_FOURCC(*'XVID')
    out = cv2.VideoWriter(filename, fourcc, 20.0, (640,480))
    try:
        stream = svr.Stream(stream_arg)
        stream.unpause()
        print("Stream Opened: {}".format(stream_arg))
    except svr.StreamException:
        del stream
        print("Stream Closed: {}".format(stream_arg))
    
    start_time = time.time()
    while (time.time() - start_time) < 2:
        record_frame(stream_arg, stream, out, filename)
