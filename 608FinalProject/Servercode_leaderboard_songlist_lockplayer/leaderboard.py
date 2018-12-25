# -*- coding: utf-8 -*-
"""
Created on Fri May 11 23:09:31 2018

@author: daiyu
"""
 
import sqlite3
import datetime
import time
leaderboard_db = '__HOME__/finalprojectleaderboard'
def request_handler(request):
    leaderboard=""
    if request['method'] == 'POST':
        conn = sqlite3.connect(leaderboard_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  # make cursor into database (allows us to execute commands)
        player=request['form']['player']
        score=request['form']['score']
        things=""
        try:
            c.execute('''CREATE TABLE leaderboardtest2 (player text, score int, timing timestamp);''') # run a CREATE TABLE command
            things = "database constructed"
        except:
            if isinstance(c.execute('''SELECT score FROM leaderboardtest2 WHERE player=?;''',(player,)).fetchone(),tuple):
                c.execute('''UPDATE leaderboardtest2 SET score = ? WHERE player = ?;''', (score,player))
            else:
                c.execute('''INSERT into leaderboardtest2 VALUES (?,?,?);''', (player,score,datetime.datetime.now()))
        things = c.execute('''SELECT player,score FROM leaderboardtest2 ORDER BY score DESC;''').fetchall()
        for ranking in things:
            leaderboard+= "," + ranking[0] + ":" + str(ranking[1])
        conn.commit() # commit commands
        conn.close() # close connection to database
        return leaderboard
    elif request['method'] == 'GET':
        conn = sqlite3.connect(leaderboard_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  # make cursor into database (allows us to execute commands)
        things = c.execute('''SELECT player,score FROM leaderboardtest2 ORDER BY score DESC;''').fetchall()
        for ranking in things:
            leaderboard+="," + ranking[0] + ":" + str(ranking[1]) 
        time.sleep(2)
        c.execute('''DELETE FROM leaderboardtest2;''')
        conn.commit() # commit commands
        conn.close() # close connection to database
        return leaderboard
