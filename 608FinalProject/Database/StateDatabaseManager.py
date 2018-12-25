import sqlite3
import datetime
import json
import ast

stateDB = '__HOME__/state.db'
def request_handler(request):
	outs = ""
	if request["method"] == "POST":
		a = request["form"]
		outs = OnPostRequest(a)
	elif request["method"] == "GET":
		outs = OnGetRequest()
	return outs



def OnPostRequest(_data):
	conn = sqlite3.connect(stateDB)  # connect to that database (will create if it doesn't already exist)
	c = conn.cursor() 
	try:
	    c.execute('''CREATE TABLE state_table (state int);''') # run a CREATE TABLE command
	    outs = "database constructed"
	except:
		c.execute('''INSERT INTO state_table VALUES (?);''', ( int(_data["state"]),))
		outs = "State Added"
	conn.commit() # commit commands
	conn.close() # close connection to database
	return outs

def OnGetRequest():
    conn = sqlite3.connect(stateDB)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()
    things = []
    things = c.execute('''SELECT * FROM state_table WHERE rowid = (SELECT MAX(rowid) FROM state_table);''').fetchall()
    out = things[0][0]
    conn.commit() # commit commands
    conn.close() # close connection to database
    return out


