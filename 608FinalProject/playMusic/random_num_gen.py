# -*- coding: utf-8 -*-
"""
Created on Wed Apr 25 15:05:01 2018

@author: arwoo
"""
import random

def request_handler(request):
    if request['method'] == 'GET':
        SongList = random.sample(range(1,61),10)
        return "The order of songs shall be {}.".format(SongList)
    else:
        return "POST requests not allowed."
    return "Request Dictionary: " + str(request)