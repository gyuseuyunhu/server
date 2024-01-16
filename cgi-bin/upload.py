#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import cgi
import cgitb  # 추가: CGI 스크립트에서 예외를 브라우저에 표시하기 위한 모듈

def delete_file():
    file_info = os.environ.get('PATH_INFO', '')
    if file_info == '':
        return
    file_path = "./www/file" + file_info
    if os.path.exists(file_path):
        try:
            os.remove(file_path)
            print("<h2>파일 삭제 성공</h2>")
            print(f"<p>파일이 성공적으로 삭제되었습니다. 삭제된 파일: {file_info}</p>")
        except:
            print("<h2>파일 삭제 실패</h2>")
            print("<p>파일을 삭제하는데 실패하였습니다.</p>")
    else:
        print("<h2>파일 삭제 실패</h2>")
        print("<p>존재하지 않는 파일입니다.</p>")

# 추가된 부분: 파일 목록 출력
def print_file_list():
    upload_dir = "./www/file"
    print("<ul>")
    for filename in os.listdir(upload_dir):
        print(f'<div><a href="/file/{filename}">{filename}</a>')
        print(f'<input type="submit" value="삭제" onClick="location.href=\'/upload.py/{filename}\'"></div><br')
    print("</ul>")

# 추가된 부분: 파일 업로드 함수
def handle_file_upload():
    form = cgi.FieldStorage()
    
    if 'file' in form:
        file_item = form['file']
        
        if file_item.filename:
            upload_dir = "./www/file"
            os.makedirs(upload_dir, exist_ok=True)
            file_path = os.path.join(upload_dir, file_item.filename)

            if (os.path.exists(file_path)):
                print("<h2>파일 업로드 실패</h2>")
                print("<p>동일한 이름의 파일이 이미 존재합니다.</p>")
                return
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

# 폼과 파일 목록 출력
print('Status: 200 OK', end="\r\n")
print('Content-type: text/html', end="\r\n\r\n")

print('''
<!DOCTYPE html>
<html lang="ko">
<head>
<meta charset="UTF-8"/>
<title>파일 저장소</title>
</head>
''')

print('''
<body>
<h1>파일 저장소</h1>
''')

# 업로드 폼 추가
print('''
<form method="post" enctype="multipart/form-data">
<label for="file">업로드할 파일을 선택하세요</label>
<br>
<input type="file" name="file" id="file" accept=".txt, .pdf, .docx, .gif, .jpeg">
<input type="submit" value="업로드">
</form>
''')

# 파일 업로드 함수 호출
if os.environ['REQUEST_METHOD'] == 'POST':
    handle_file_upload()
      
# 파일 삭제 함수 호출
if os.environ['REQUEST_METHOD'] == 'DELETE':
    delete_file()

# 파일 목록 출력
print_file_list()
    
print('''</body>
</html>''')