#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import cgi
import cgitb  # 추가: CGI 스크립트에서 예외를 브라우저에 표시하기 위한 모듈

# 추가된 부분: 파일 목록 출력
def print_file_list():
    upload_dir = "./www/file"
    print("<ul>")
    for filename in os.listdir(upload_dir):
        print(f'<div><a href="/file/{filename}">{filename}</a></div>')
    print("</ul>")

# 추가된 부분: 파일 업로드 함수
def handle_file_upload():
    request_method = os.environ.get('REQUEST_METHOD', 'GET')
    if request_method != 'POST':
        return 
    form = cgi.FieldStorage()
    
    if 'file' in form:
        file_item = form['file']
        
        if file_item.filename:
            upload_dir = "./www/file"
            os.makedirs(upload_dir, exist_ok=True)
            file_path = os.path.join(upload_dir, file_item.filename)

            with open(file_path, 'wb') as file:
                file.write(file_item.file.read())

            # 파일 업로드 성공 메시지 출력
            print("<h2>파일 업로드 성공</h2>")
            print(f"<p>파일이 성공적으로 업로드되었습니다. 저장된 파일: <a href='/file/{file_item.filename}'>{file_item.filename}</a></p>")
        else:
            print("<h2>파일 업로드 실패</h2>")
            print("<p>올바른 파일을 선택하세요.</p>")
    else:
        print("<h2>파일 업로드 실패</h2>")
        print("<p>파일이 선택되지 않았습니다.</p>")

# 추가된 부분: 예외 처리
try:
    cgitb.enable()  # 추가: CGI 스크립트에서 예외를 브라우저에 표시하기 위한 설정

    # 폼과 파일 목록 출력
    print("Content-type: text/html\r\n\r\n")
    print('<html lang=\"ko\">')
    print('<head>')
    print('<meta charset="UTF-8"/>')
    print('<title>파일 저장소</title>')
    print("</head>")
    print("<body>")
    print("<h2>파일 목록</h2>")

    # 업로드 폼 추가
    print('''<form method="post" enctype="multipart/form-data">
          <label for="file">업로드할 파일을 선택하세요</label>
          <br>
          <input type="file" name="file" id="file" accept=".txt, .pdf, .docx, .gif, .jpeg">
          <br>
          <input type="submit" value="업로드">
          </form>''')

    # 추가된 부분: 파일 업로드 함수 호출
    handle_file_upload()

    # 파일 목록 출력
    print_file_list()
    
    print("</body>")
    print("</html>")
except Exception as e:
    print(f"Content-type: text/html\r\n\r\n")
    print(f"<html><body><h2>오류 발생:</h2><p>{str(e)}</p></body></html>")
