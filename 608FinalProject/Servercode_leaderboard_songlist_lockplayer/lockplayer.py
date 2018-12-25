# -*- coding: utf-8 -*-
"""
Created on Mon May 14 14:21:11 2018

@author: daiyu
"""
import sqlite3
lockplayer_db='__HOME__/lockeplayer'
def request_handler(request):
    if request['method'] == 'POST':
        conn = sqlite3.connect(lockplayer_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()   
        if request['form']['newlock']=="newlock":
            try:
                c.execute('''CREATE TABLE lockplayer1 (locked int);''') # run a CREATE TABLE command
            except:
                locked=c.execute('''SELECT * FROM lockplayer1;''').fetchone()
                c.execute('''UPDATE lockplayer1 SET locked = ?;''', (locked[0]+1,))
        elif request['form']['newlock']=="reset":
            c.execute('''UPDATE lockplayer1 SET locked = ?;''', (0,))
            locked=c.execute('''SELECT * FROM lockplayer1;''').fetchone()
        locked=c.execute('''SELECT * FROM lockplayer1;''').fetchone()
        conn.commit() # commit commands
        conn.close() # close connection to database
        return locked[0]
    elif request['method'] == 'GET':
        conn = sqlite3.connect(lockplayer_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  
        things = c.execute('''SELECT * FROM lockplayer1;''').fetchone()
        conn.commit() # commit commands
        conn.close() # close connection to database
        return things[0]
    
    