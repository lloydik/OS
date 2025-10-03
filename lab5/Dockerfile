FROM alpine:latest

WORKDIR /app
COPY . /app

RUN apk add --no-cache \
    build-base \
    make \
    gcc \
    g++

CMD ["/bin/sh"]