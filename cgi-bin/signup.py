#!/usr/bin/env python3
import os
import json
import cgi
import random
import string
from datetime import datetime, timedelta

dictionary_file_path = os.environ.get('WEBSERV_ROOT', '') + os.environ.get('UPLOAD_DIR', '/') + "member.json"

def load_dictionary():
    try:
        with open(dictionary_file_path, 'r') as file:
            return json.load(file)
    except (IOError, ValueError):
        return {}

def generate_random_string(length=20):
    # 대소문자 알파벳과 숫자를 사용하여 랜덤한 문자열 생성
    characters = string.ascii_letters + string.digits
    random_string = ''.join(random.choice(characters) for _ in range(length))
    return random_string

random_string = generate_random_string()
nickname_dictionary = load_dictionary()

def add_nickname_to_dictionary(cookie, nickname):
    nickname_dictionary[cookie] = nickname
    with open(dictionary_file_path, 'w') as file:
        json.dump(nickname_dictionary, file)

form = cgi.FieldStorage()
nickname = form.getvalue('nickname')

add_nickname_to_dictionary(random_string, nickname)

expiration_date = datetime.utcnow() + timedelta(days=1)
expires_str = expiration_date.strftime('%a, %d %b %Y %H:%M:%S GMT')

print('Status: 200 OK', end='\r\n')
print(f'Set-Cookie: yourCookieName={random_string}; Expires={expires_str}; Path=/', end='\r\n')
print("Content-type: text/plain", end='\r\n\r\n')
