FROM alpine:3.12

RUN apk update && apk upgrade && apk add --no-cache dumb-init g++ make

WORKDIR /webserv

COPY ./src ./src

COPY ./include ./include

COPY ./www ./www

COPY ./YoupiBanane ./YoupiBanane

COPY ./cgi-bin ./cgi-bin

COPY ./Makefile ./Makefile

RUN make && make clean

ENV WEBSERV_ROOT=/webserv

EXPOSE 80

ENTRYPOINT ["dumb-init", "--", "tail", "-f", "/dev/null"] 
