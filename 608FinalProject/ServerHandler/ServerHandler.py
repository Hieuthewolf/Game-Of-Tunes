import requests
import sqlite3

songDatabase = '__HOME__/SongDatabase.db'

def request_handler(request):
    result = ""
    if request['method'] == "GET":
        if request['values']['state'] == "checkguess":
            songIndex = request['values']['songIndex']
            guess = request['values']['guess']
            result = OnCheckGuess(songIndex, guess)
        if request['values']['state'] == "sendHint":
            songIndex = request['values']['songIndex']
            hintNum = request['values']['hintNum']
            result = OnHintGetRequest(songIndex, hintNum)
        return result

def OnCheckGuess(_songIndex, _guess):
    conn = sqlite3.connect(songDatabase)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()
    isGuessCorrect = ""
    #song = ""
    songRows = c.execute('''SELECT * FROM SongTable WHERE id == (?);''',(_songIndex,))
    for songRow in songRows:
        songName = songRow[1]
    if songName.lower() == _guess.lower():
        isGuessCorrect = "TRUE"
    else:
        isGuessCorrect = "FALSE"
    conn.commit() # commit commands
    conn.close() # close connection to database
    return isGuessCorrect

def OnHintGetRequest(_songIndex, _hintNum):
    conn = sqlite3.connect(songDatabase)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()
    songHint = ""
    songRows = c.execute('''SELECT * FROM SongTable WHERE id == (?);''',(_songIndex,))
    for songRow in songRows:
        if _hintNum == "1":
            songHint = "This is a song by " + songRow[2]
        if _hintNum == "2":
            songHint = "This is a song on the album " + songRow[3]
    conn.commit() # commit commands
    conn.close() # close connection to database
    return songHint
