FROM python:3.7-alpine

RUN apk add cmake make
RUN apk add --no-cache --virtual .build-deps gcc g++ musl-dev && pip install cython cget