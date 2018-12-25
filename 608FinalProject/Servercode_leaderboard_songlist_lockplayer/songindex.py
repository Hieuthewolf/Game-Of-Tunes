# -*- coding: utf-8 -*-
"""
Created on Mon May 14 13:38:24 2018

@author: daiyu
"""
import sqlite3
import random
SongList_db='__HOME__/SongList'
def request_handler(request):
    if request['method'] == 'POST':
        conn = sqlite3.connect(SongList_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  
        index=""
        SongList = random.sample(range(1,61),10)
        for i in SongList:
            index+=","+str(i)
        try:
            c.execute('''CREATE TABLE songindex3 (SongList text);''') # run a CREATE TABLE command
        except:
            c.execute('''DELETE FROM songindex3;''')
            c.execute('''INSERT into songindex3 VALUES (?);''', (index,))
        conn.commit() # commit commands
        conn.close() # close connection to database
    elif request['method'] == 'GET':
        conn = sqlite3.connect(SongList_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  
        things = c.execute('''SELECT * FROM songindex3;''').fetchall()
        conn.commit() # commit commands
        conn.close() # close connection to database
        return things[0][0]
    
    