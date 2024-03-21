# webserv

## Team
<table align="center">
  <th>gyuhong</th>
  <th>seulee2</th>
  <th>yunjcho</th>
  <th>hujeong</th>
  <tr>
    <td><img src="https://avatars.githubusercontent.com/u/108771739?v=4" width="140" height="140"/></td>
    <td><img src="https://avatars.githubusercontent.com/u/96572410?v=4" width="140" height="140"/></td>
    <td><img src="https://avatars.githubusercontent.com/u/73283078?v=4" width="140" height="140"/></td>
    <td><img src="https://avatars.githubusercontent.com/u/114281631?v=4" width="140" height="140"/></td>
  </tr>
  <tr>
    <td align="center"><a href="https://github.com/guune">홍규선</a>
    </td>
    <td align="center"><a href="https://github.com/sngsho">이승효</a>
    </td>
    <td align="center"><a href="https://github.com/YunjooCho">조윤주</a>
    </td>
    <td align="center"><a href="https://github.com/heehoh">정희호</a>
    </td>
  </tr>
</table>

## About
HTTP/1.1 프로토콜 기반으로 소켓 통신을 하는 웹서버를 C++로 구현하는 프로젝트입니다.  
웹서버를 가상 호스팅, 라우팅 등을 설정 파일을 통해 동작시킬 수 있습니다.  
I/O Multiplexing 함수인 kqueue()를 사용해서 들어오는 요청을 비동기적으로 처리합니다.

## Overview


https://github.com/gyuseuyunhu/server/assets/114281631/0d8aa302-ed84-41e0-aab0-7b540dbfd2b9



## Usage
이 프로젝트는 mac os 환경에서 동작합니다.
```
make
export WEBSERV_ROOT=$PWD
./webserv [Config File]
```
docker를 이용해서 윈도우나 리눅스 환경에서도 테스트 가능합니다.
```
git switch docker // 도커 브랜치로 이동해야 합니다.
docker compose up -d
```
웹 브라우저로 127.0.0.1(포트 80)으로 접속하여 테스트할 수 있습니다.

## Flow
<img width="1052" alt="flow" src="https://github.com/gyuseuyunhu/server/assets/114281631/50c9bbc0-903e-4e12-a969-29f1a98b8d3d">

- 설정 파일을 파싱해서 port, host, location 등 설정대로 웹 서버를 동작시킵니다.
- 클라이언트가 연결을 요청하면 요청을 수락하고 연결을 맺습니다.
- 요청을 맺은 후 클라이언트의 메시지를 해석하고 클라이언트의 요청 url이 일반 파일인지 CGI인지 확인합니다.
- 일반 파일에 대한 요청이면 파일을 읽은 후 응답 메시지에 담아 클라이언트에게 전송합니다.
- CGI 요청이면 CGI 프로세스를 동작시킨 후 CGI 프로세스와 통신하여 응답 메시지를 생성하고 클라이언트에게 전송합니다.
- 클라이언트에게 응답 메시지를 전송한 뒤 다시 클라이언트의 요청 메시지를 기다립니다(keep-alive).

## Detail
### 설정 파일
#### 가상 호스팅
- 하나의 웹서버에서 여러 도메인 이름을 호스팅하는 방법입니다.
- 크게 이름 기반(name-based), IP 기반(IP-based) 가상 호스팅으로 나뉩니다.
- 이름 기반 가상 호스팅에서는 같은 IP 주소에 대해 여러 호스트 이름을 사용할 수 있고, 클라이언트는 요청메시지에 Host 헤더를 명시해야 합니다.
- IP 기반 가상 호스팅은 웹사이트마다 다른 IP 주소 또는 Port를 가지는 방식입니다.
- 이 프로젝트는 이름 기반 가상 호스팅을 지원하며, nginx랑 유사하게 server block 내의 server_name을 다르게 함으로써 작동합니다.
#### URI 매칭 및 처리
- URI 요청 수신: 요청이 들어오면 이름 기반 호스팅에 따라 적절한 server block을 찾습니다.
- location block 매칭: 요청된 URI와 부분 일치되는 server block 내의 location block URI를 찾습니다.
- URI 처리: 찾은 location block의 지시어에 따라 요청된 URI를 처리합니다.
- 301 redirection 처리: 특정 조건에 따라 요청된 URI에 대해 301 redirection을 처리할 수 있습니다. 1. return 지시어가 있는 경우 2. 같은 URI에 대한 요청이 ＇/＇ 없이 들어온 경우, ＇/＇가 추가된 URI로 처리됩니다.
- 기본 server block 처리: 적절한 server block을 찾지 못하면 첫 번째 server block으로 처리됩니다.
- 기본 location block 처리: 적절한 location block을 찾지 못하면 해당 server block의 지시어에 따라 처리됩니다.
- 정규식 지원 여부: 정규식을 사용한 URI 매칭을 지원하지 않습니다.
#### 지시어(directive)
구성파일 내에 다음의 지시어들을 지원합니다.
- listen: 서버가 클라이언트의 요청을 수신할 포트 번호를 지정합니다. 예: listen 80;
- server_name: 서버의 도메인 이름을 지정합니다. 예: server_name example.com;
- root: nginx의 root보다는 alias에 가깝습니다. 대체할 경로를 지정합니다.
- 예: location /i/ { root /data/w3/images/; } 이었을 때 “/i/top. gif”는 ＂/data/w3/images/top. gif＂가 됩니다.
- index: 서버에 요청이 들어왔을 때 제공할 기본 파일을 제공합니다. 기본값은 index.html입니다. 예: index test.html;
- error_page: 특정 HTTP 에러 코드에 대해 사용자에게 제공할 페이지를 지정합니다.
- error_page 지시어가 없거나 에러 페이지를 open 하는데 에러가 나면 하드 코딩된 기본 에러 페이지를 제공합니다. 예: error_page 404. html
- client_max_body_size: 클라이언트 요청의 max body 크기를 제한합니다. 최대 크기는 2GB입니다. M, K, G 유닛을 지원합니다. 예: client_max_body_size 8M;
- return: 요청에 대한 응답으로 301 redirection 합니다. return www.example.com;
- autoindex: 폴더를 요청했을 때 디렉터리 내용을 나열할 건지 결정합니다. 기본값은 off입니다. 예: autoindex on;
- limit_except: 특정 HTTP 메서드를 허용하거나 금지합니다. 기본값은 get, post 허용입니다. 예: limit_except deny POST;
- cgi_extension: 처리할 CGI 파일의 확장자를 지정합니다. 예: cgi_extension .cgi;
- cgi_path: 처리할 CGI 파일 위치를 지정합니다. 기본값은 ＇/＇ 입니다. 예: /cgi-bin/login.py;
- cgi_upload_dir: 파일 올릴 폴더를 지정합니다. 예: /www/member/;
  
- 각 블록 유형(HTTP, server, location)에 따라 사용할 수 있는 지시어가 다르며, 하위 블록에 같은 이름의 지시어가 선언되면, 해당 지시어는 상위 블록의 설정을 덮어쓰고 새로운 값을 적용합니다.
  
| 지시어                | HTTP_BLOCK | SERVER_BLOCK | LOCATION_BLOCK | 중복 허용 여부 |
|----------------------|------------|--------------|----------------|---------------|
| root                 | O          | O            | O              | X             |
| index                | O          | O            | O              | O             |
| error_page           | O          | O            | O              | O             |
| client_max_body_size | O          | O            | O              | X             |
| listen               | X          | O            | X              | X             |
| server_name          | X          | O            | X              | X             |
| return               | X          | O            | O              | X             |
| autoindex            | X          | X            | O              | X             |
| limit_except         | X          | X            | O              | O             |
| cgi_extension        | X          | X            | O              | X             |
| cgi_path             | X          | X            | O              | X             |
| cgi_upload_dir       | X          | X            | O              | X             |
- 이 표에서 ＂O＂는 해당 블록에서 지시어가 허용됨을, ＂X＂는 허용되지 않음을 나타냅니다.
- ＂중복 허용 여부＂는 해당 지시어가 같은 블록 내에서 여러 번 선언될 수 있는지를 나타냅니다.

### I/O Multiplexing
- I/O Multiplexing으로 여러 클라이언트의 요청을 동시에 처리합니다.
- I/O Multiplexing은 커널에서 fd를 감시하고 fd에 처리할 데이터가 발생했을 때 유저 프로세스에게 callback 신호가 오고 이를 처리하는 방식입니다.
- 데이터를 처리할 수 있을 만큼만 처리하기 때문에 여러 요청을 동시적으로 다룰 수 있습니다.
```
mKq = kqueue();
```
- kqueue 함수는 event를 저장할 새로운 queue를 커널에 요청하는 함수입니다. 해당 큐의 파일 디스크립터를 반환합니다.

```
int n = kevent(mKq, &amp;mNewEvents[0], mNewEvents.size(), mHandleEvents, MAX_EVENT_CNT, NULL);
for (int i = 0; i &amp;lt; n; ++i)
{
    reinterpret_cast&amp;lt;AEvent *&gt;(mHandleEvents[i].udata)-&gt;handle();
}
```
- kevent 함수에서 mNewEvents로 감시해야 하는 이벤트를 커널 큐에 등록시키고, mHandleEvents로 데이터가 발생한 요청을 반환받아 반환된 요청을 처리합니다.

### HTTP 메시지
- HTTP 메시지는 요청 메시지와 응답 메시지 두 종류로 분류할 수 있습니다.
```
// 요청 메시지
GET / HTTP/1.1
Host: localhost

// 응답 메시지
HTTP/1.1 200 OK
Content-type: text/html; =UTF-8
Content-length: 2134

(바디 생략)
```
- HTTP 메시지는 시작줄 헤더 바디로 나누어 해석합니다.
- HTTP 요청 메시지의 시작 줄은 메소드, 경로, 프로토콜로 구성되어 있습니다.
- 이 프로젝트는 GET, POST, DELETE 메소드를 지원하며, HTTP/1.1에 따라 응답을 보낸 뒤에도 소켓 연결을 지속합니다.
- 헤더의 Host와 시작줄의 경로를 확인하고, 요청에 따른 응답 메세지를 시작줄 헤더 바디로 구성해서 소켓을 통해 전송합니다.
- Host 헤더는 필수 필드로 요청 메시지 Host 헤더가 없을 시 400 Bad Request를 응답합니다.
- Content-length로 전달된 길이만큼 본문을 읽습니다.
- 응답 메시지의 시작줄 상태코드로 요청이 성공적으로 완료되었는지 확인할 수 있고, 이 프로젝트는 200번대부터 500번대까지 다양한 상태코드를 지원합니다.

### CGI
- 웹서버가 요청마다 동적인 페이지를 생성해주기 위해 CGI를 사용했습니다.
- 유저가 CGI에 해당하는 url을 전송했을 때 CGI에 해당하는 디렉토리일 경우 웹서버는 파일을 읽어 전송하는 것이 아닌 프로그램을 실행시키고 프로그램의 출력이 클라이언트에게 전달합니다.
- HTTP 헤더 내용은 환경변수로 변환되어 CGI 프로세스에 전달되고 요청 본문(BODY)은 표준입력을 통해 CGI 프로세스에 전달됩니다.
- 동적으로 상호 작용하는 POST나 DELETE 같은 메소드는 CGI로 처리한 다음 결과를 웹서버가 클라이언트에게 반환합니다.
- CGI 프로세스는 다양한 언어로 개발될 수 있고 이 프로젝트는 python과 perl을 사용했습니다.

## 참고자료
- [가상 호스팅](https://en.wikipedia.org/wiki/Virtual_hosting)
- [HTTP 메세지](https://developer.mozilla.org/ko/docs/Web/HTTP/Messages)
- [URI 매칭 및 처리](https://nginx.org/en/docs/http/ngx_http_core_module.html#location)
- [지시어](https://nginx.org/en/docs/http/ngx_http_core_module.html#location)
- [I/O Multiplexing](https://blog.naver.com/n_cloudplatform/222189669084)
- [kqueue](https://blog.naver.com/n_cloudplatform/222255261317)
- [CGI](https://electricalfundablog.com/common-gateway-interface-cgi/)
