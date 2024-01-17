#!/usr/bin/env python3
import json
import os
import cgi

# 딕셔너리 파일 경로
directory = os.environ.get('WEBSERV_ROOT', '') + os.environ.get('UPLOAD_DIR', '/')
dictionary_file_path = directory + "member.json"

def load_dictionary():
		if not os.path.exists(directory):
			os.makedirs(directory)
		try:
			with open(dictionary_file_path, 'r') as file:
				return json.load(file)
		except (IOError, ValueError):
				return {}

# 딕셔너리 파일 로드
nickname_dictionary = load_dictionary()

# 클라이언트로부터 전달된 쿠키 값 가져오기
form = cgi.FieldStorage()
user_cookie = form.getvalue('cookie')

if not nickname_dictionary:
    # 딕셔너리 파일이 없는 경우
    print(f'Status: 200 OK', end='\r\n')
    print("Content-type: text/plain", end='\r\n\r\n')
    print("non-exist")
elif user_cookie in nickname_dictionary:
    # 딕셔너리에 쿠키 값이 있는 경우 해당 닉네임 반환
    user_nickname = nickname_dictionary[user_cookie]
    print(f'Status: 200 OK', end='\r\n')
    print("Content-type: text/plain", end='\r\n\r\n')
    print(user_nickname)
else:
    # 딕셔너리에 쿠키 값이 없는 경우
    print(f'Status: 200 OK', end='\r\n')
    print("Content-type: text/plain", end='\r\n\r\n')
    print("non-exist")
