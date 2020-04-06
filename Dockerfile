FROM debian:buster AS build
ARG BUILD_DIR=/build
RUN mkdir -p ${BUILD_DIR}/bin && \
    (for f in 1 3 5 8; do mkdir -p ${BUILD_DIR}/man/man${f}; done) && \
    apt-get update && \
    apt-get install -y gcc g++ make flex bison curl unzip patch

COPY z80-asm.patch /src/z80-asm/
RUN cd /src/z80-asm && \
    curl --silent --show-error --location \
         --url http://wwwhomes.uni-bielefeld.de/achim/z80-asm/z80-asm-current.tar.gz \
         --output z80-asm-current.tar.gz && \
    tar -zxf z80-asm-current.tar.gz && \
    cd z80-asm-* && \
    patch -p1 <../z80-asm.patch && \
    BIN_DIR=${BUILD_DIR}/bin MAN_DIR=${BUILD_DIR}/man make -e -b all install

RUN mkdir -p /src/zmac && \
    cd /src/zmac && \
    curl --silent --show-error --location --url http://48k.ca/zmac.zip \
         --output zmac.zip && \
    unzip zmac.zip && \
    cd src && \
    make clean zmac && \
    cp zmac ${BUILD_DIR}/bin/
  
COPY ld80.patch /src/ld80/
RUN cd /src/ld80 && \
    curl --silent --show-error --location --url http://48k.ca/ld80.zip \
         --output ld80.zip && \
    unzip ld80.zip && \
    patch < ld80.patch && \
    make && \
    cp ld80 ${BUILD_DIR}/bin/ && \
    cp ld80.1 ${BUILD_DIR}/man/man1/

COPY z80-bin/ /src/z80-bin
RUN cd /src/z80-bin && \
    make clean all && \
    cp z80-bin ${BUILD_DIR}/bin/

COPY z80-ports/ /src/z80-ports
RUN cd /src/z80-ports && \
    make clean all && \
    cp z80-ports ${BUILD_DIR}/bin/

COPY z80-console/ /src/z80-console
RUN cd /src/z80-console && \
    make clean all && \
    cp z80-console ${BUILD_DIR}/bin/

FROM debian:buster
RUN mkdir /Z80 && \
    apt-get update && \
    apt-get install -y gcc make sdcc binutils vim man
COPY --from=build /build /usr/local
WORKDIR /Z80
CMD [ "/bin/bash" ]
