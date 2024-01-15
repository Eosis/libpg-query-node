FROM node:20-alpine
RUN apk --no-cache add git
RUN apk add --no-cache bash
RUN apk add --no-cache make
RUN apk add --update alpine-sdk
RUN apk add --no-cache python3
RUN apk add --no-cache protoc
RUN apk add --no-cache protobuf-c-dev
ENTRYPOINT ["/bin/bash"]
