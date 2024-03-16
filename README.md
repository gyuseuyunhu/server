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
HTTP/1.1 프로토콜 기반으로 소켓 통신을 하는 웹서버를 C++로 구현하는 프로젝트 입니다.  
웹서버를 가상호스팅, 라우팅 등을 설정 파일을 통해 동작시킬 수 있습니다.  
I/O Multiplexing인 kqueue() 사용해서 들어오는 요청을 비동기적으로 처리합니다.  

## Usage
이 프로젝트는 mac os 환경에서 동작합니다.
```
make
export WEBSERV_ROOT=$PWD
./webserv [Config File]
```

## Flow
<img width="1052" alt="flow" src="https://github.com/gyuseuyunhu/server/assets/114281631/50c9bbc0-903e-4e12-a969-29f1a98b8d3d">


## Detail
### 설정 파일
#### 가상 호스팅
- 하나의 웹서버에서 여러 도메인 이름을 호스팅하는 방법입니다.
- 크게 이름 기반(name-based), IP 기반(IP-based) 가상 호스팅으로 나뉩니다.
- 이름 기반 가상 호스팅은 동일한 IP 주소에 대해 여러 호스트 이름을 사용하며, 더 구체적으로 말하면 HTTP/1.1에서 필수인 Host HTTP 헤더를 설정해서 사용하는것을 의미합니다.
- IP 기반 가상 호스팅은 각 웹사이트마다 다른 IP 주소 또는 Port를 가지는 방식입니다.
- 이 프로젝트의 경우 이름 기반 가상 호스팅을 사용했으며, nginx랑 유사하게 서버블록 내의 server_name을 다르게 함으로써 작동합니다.




## 참고자료
- [가상 호스팅](https://en.wikipedia.org/wiki/Virtual_hosting)

