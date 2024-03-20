FROM frolvlad/alpine-glibc

RUN apk update && apk upgrade && apk add --no-cache dumb-init g++ make python3 perl perl-cgi

WORKDIR /webserv

COPY ./src ./src

COPY ./include ./include

COPY ./www ./www

COPY ./YoupiBanane ./YoupiBanane

COPY ./cgi-bin ./cgi-bin

COPY ./Makefile ./Makefile

RUN make && make clean && rm ./cgi-bin/cgi_tester && mv ./cgi-bin/ubuntu_cgi_tester ./cgi-bin/cgi_tester

ENV WEBSERV_ROOT=/webserv

EXPOSE 80

ENTRYPOINT ["dumb-init", "--", "./webserv"] 
