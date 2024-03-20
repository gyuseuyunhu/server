#!/usr/bin/env perl

use CGI;

my $cgi = new CGI;

if ($cgi->request_method eq 'POST') 
{
my $title = $cgi->param('title') || 'Let it Snow ☃️ Let it Snow ☃️';
my $emojiInput = $cgi->param('emojiInput') || '❄️';
my $textColor = $cgi->param('textColor') || '#99ccff';
my $beginColor = $cgi->param('beginColor') || '#172c43';
my $endColor = $cgi->param('endColor') || '#090a0f';

print $cgi->header(-status => '200 OK', -type => 'text/html', -charset => 'utf-8');
print <<HTML;
<!DOCTYPE html>
<html lang="ko">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link href="https://fonts.googleapis.com/css2?family=Stylish&display=swap" rel="stylesheet">
    <title>눈 내리는 페이지</title>
    <style>
      body {
        height: 100vh;
        background: radial-gradient(
          ellipse at bottom,
          $beginColor 0%,
          $endColor 100%
        );
        overflow: hidden;
      }
      .snow {
        width: 20px;
        height: 20px;
        opacity: 0;
        animation: snowFall 10s linear infinite;
      }
      \@keyframes snowFall {
        0% {
          opacity: 0;
          transform: translateY(0);
        }
        20% {
          opacity: 1;
          transform: translate(-50px, 20vh);
        }
        40% {
          opacity: 1;
          transform: translate(50px, 40vh);
        }
        60% {
          opacity: 1;
          transform: translate(-50px, 60vh);
        }
        80% {
          opacity: 1;
          transform: translate(0px, 80vh);
        }
        100% {
          opacity: 1;
          transform: translateY(100vh);
        }
      }
      .snow:nth-of-type(2n) {
        animation-delay: 0s;
        animation-duration: 15s;
      }
      .snow:nth-of-type(2n + 1) {
        animation-delay: 5s;
        animation-duration: 15s;
      }
      .title {
        font-family: "Stylish", sans-serif;
        font-size: 100px;
        color: $textColor;
        text-align: center;
        position: absolute;
        top: 30%;
        width: 100%;
      }
      \@media screen and (max-width: 600px) {
        .title {
          font-family: "Stylish", sans-serif;
          font-size: 35px;
          top: 30%;
        }
      }
    </style>
  </head>
  <body>
    <script>
      function createSnow(snow) {
        const el = document.createElement("div");
        el.className = "snow";
        el.style.left = randomPosition() + "px";
        el.textContent = snow;
        el.style.fontSize = "30px";
        el.style.marginLeft = randomPosition() + "px";
        document.body.appendChild(el);
      }
			function randomPosition() {
        return Math.floor(Math.random() * window.innerWidth);
      }
      for (let i = 0; i < 300; i++) {
        createSnow("$emojiInput");
      }
    </script>
    <div class="title">$title</div>
  </body>
</html>
HTML

} 
elsif ($cgi->request_method eq 'GET') 
{
print $cgi->header(-status => '200 OK', -type => 'text/html', -charset => 'utf-8');
print <<HTML;
<!DOCTYPE html>
<html lang="ko">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>눈 내리는 페이지 만들기</title>
    <style>
        body {
            background-color: #f0f0f0; /* 페이지 전체 배경색 */
            color: #333; /* 페이지 전체 텍스트 색상 */
            font-family: 'Arial', sans-serif; /* 글꼴 설정 */
            margin: 20px; /* 페이지 전체 여백 */
        }

        h1 {
            color: #99CCFF; /* 헤더 제목 색상 */
						text-align: center; /* 헤더 제목 가운데 정렬 */
        }

        form {
            max-width: 500px; /* 폼의 최대 너비 설정 */
            margin: auto; /* 중앙 정렬 */
            padding: 20px; /* 폼 내부 여백 */
            background-color: #f2f2f2; /* 폼 배경색 */
            border-radius: 10px; /* 폼 테두리 둥글게 */
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* 그림자 효과 */
        }

        label {
       /* 라벨을 블록 요소로 표시하여 줄바꿈이 일어나도록 설정 */
            margin-bottom: 5px; /* 라벨 아래 여백 */
            color: #333; /* 라벨 텍스트 색상 */
        }

        input {
            margin-bottom: 10px; /* 입력 필드 아래 여백 */
            border: 1px solid #ccc; /* 입력 필드 테두리 스타일 */
            border-radius: 5px; /* 입력 필드 테두리 둥글게 */
            box-sizing: border-box; /* 입력 필드 크기 계산에 테두리와 패딩 포함 */
        }
				#title {
					height: 40px;
				}

				#emojiInput {
					width: 20px;
					height: 20px;
				}

        input[type="submit"] {
						height: 40px;
						border: none; /* 제출 버튼 테두리 없음 */
						border-radius: 5px; /* 제출 버튼 테두리 둥글게 */
						text-align: center; /* 제출 버튼 텍스트 가운데 정렬 */
            background-color: #99CCFF; /* 제출 버튼 배경색 */
            color: #fff; /* 제출 버튼 텍스트 색상 */
            cursor: pointer; /* 포인터 형태로 변경하여 사용자에게 클릭 가능한 요소임을 알림 */
        }

        input[type="submit"]:hover {
            background-color: #79EDFF; /* 마우스 호버 시 배경색 변경 */
        }

    </style>
</head>
<body>
    <h1>눈 내리는 페이지 만들기 ☃️</h1>
    <form action="/perl/upload.pl" method="post">
        <label for="title">페이지 글귀: </label>
        <input type="text" id="title" name="title" size="50" value="Let it Snow ☃️ Let it Snow ☃️">
        <br><br>
        <label for="textColor">글자 색: </label>
        <input type="color" id="textColor" name="textColor" value="#99ccff">
        &nbsp;&nbsp;
        <label for="beginColor">배경 시작 색:</label>
        <input type="color" id="beginColor" name="beginColor" value="#172c43">
        &nbsp;&nbsp;
        <label for="endColor">배경 끝 색: </label>
        <input type="color" id="endColor" name="endColor" value="#090a0f">
        <br><br>
        <label for="emojiInput" >이모지 입력:</label>
        <input type="text" id="emojiInput" name="emojiInput" size="2" value="❄️">
        <br><br>
        <input type="submit" value="페이지 만들기">
    </form>
</body>
</html>
HTML
} 
else 
{
print $cgi->header(-status => '405 Method Not Allowed', -type => 'text/html', -charset => 'utf-8');
print <<HTML;
<!DOCTYPE html>
<html lang="ko">
<head>
		<meta charset="UTF-8">
		<title>405 Method Not Allowed</title>
</head>
<body>
		<h1>405 Method Not Allowed</h1>
		<p>지원하지 않는 요청입니다.</p>
</body>
</html>
HTML
}