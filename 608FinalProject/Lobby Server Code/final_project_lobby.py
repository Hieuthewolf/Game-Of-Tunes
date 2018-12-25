#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Apr  3 10:11:38 2018

@author: hieunguyen
"""

import sqlite3
import datetime
import random

from threading import Timer

import numpy

#sftp hieutan@iesc-s1.mit.edu
#6839087

#x = [2, 3, 4]
#y = [9, 14, 21]

#(2, 9)
#(3, 14)
#(4, 21)

#y = x^2 + 5 

y_intercept = 5

visits_db = '__HOME__/final_project_lobby.db'
 
def request_handler(request):
    conn = sqlite3.connect(visits_db)
    c = conn.cursor()
    forty_five_seconds_ago = datetime.datetime.now()- datetime.timedelta(seconds = 45) # create time for fifteen minutes ago!
    
    if request['method'] == 'POST':
        
       if 'user' not in request['form']:
           return "You must enter a Username."
       
       user = str(request['form']['user'])
       
       try:
           c.execute('''CREATE TABLE SSSS (user text, timing timestamp);''')
       except:
           c.execute('''INSERT into SSSS VALUES (?, ?);''', (user, datetime.datetime.now()))
           conn.commit()
           conn.close()
           
    if request['method'] == 'GET': 
       beginning_list = []
       beginning_list.append("Players: ")
       items = c.execute('''SELECT user FROM SSSS WHERE timing > ? ORDER BY timing DESC;''',(forty_five_seconds_ago,)).fetchall()
       for things in items:
           beginning_list.append(things[0])  
           
       user_list = beginning_list[1:len(beginning_list)]

       if len(user_list) == 5:
           return "success"
       
       list_stringified = ",".join(beginning_list)
       return str(list_stringified)
   
      
   
       
   
    
   
       
   

    
   


   
   
       

        
        
