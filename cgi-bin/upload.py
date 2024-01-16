#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import cgi
# 파일 삭제 메소드
def delete_file():
    file_info = os.environ.get('PATH_INFO', '')
    if file_info == '':
        return
    upload_dir = os.environ.get('UPLOAD_DIR', '/')
    webserv_root = os.environ.get('WEBSERV_ROOT', '')
    file_path = webserv_root + upload_dir + file_info
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

# 파일 목록 출력
def print_file_list():
    upload_dir = os.environ.get('WEBSERV_ROOT', '') + os.environ.get('UPLOAD_DIR', '/')
    print("<ul>")
    for filename in os.listdir(upload_dir):
        print(f'<div><a href="/file/{filename}">{filename}</a>')
        print(f'''
            <form id="deleteForm" method="DELETE" action="python/upload.py/{filename}">
                <input type="hidden" name="_method" value="DELETE">
                <input type="submit" value="삭제">
            </form>
        </div>
				''')
    print("</ul>")

# 파일 업로드 함수
def handle_file_upload():
    form = cgi.FieldStorage()
    
    if 'file' in form:
        file_item = form['file']
        
        if file_item.filename:
            upload_dir = os.environ.get('WEBSERV_ROOT', '') + os.environ.get('UPLOAD_DIR', '/')
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
status = 200
code = "OK"
print(f'Status: {status} {code}', end="\r\n")
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
if os.environ['REQUEST_METHOD'] == 'GET':
		print_file_list()

# 파일 삭제 메소드
print('''
<script>
document.addEventListener('DOMContentLoaded', function() {
    var deleteForms = document.querySelectorAll('#deleteForm');
    deleteForms.forEach(function(form) {
        form.addEventListener('submit', function(event) {
            event.preventDefault();
            var xhr = new XMLHttpRequest();
            xhr.open('DELETE', form.action, true);
            xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
            xhr.onload = function() {
                if (xhr.status === 200) {
                    // 성공적으로 삭제되면 페이지를 다시 로드
										alert("파일이 성공적으로 삭제되었습니다")
                    location.reload();
                }
            };
            xhr.send('_method=DELETE');
        });
    });
});
</script>
''')
    
print('''</body>
</html>''')