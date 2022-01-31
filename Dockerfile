FROM devkitpro/devkitppc:20220128 AS libiosuhax-build

WORKDIR /libiosuhax
RUN git init && \
    git remote add origin https://github.com/wiiu-env/libiosuhax && \
    git fetch --depth 1 origin 9faa95af562091644986d3ccf47500c7d65c74e6 && \
    git checkout FETCH_HEAD && \
    git submodule update --init --depth 1

RUN make install DESTDIR=/out

FROM devkitpro/devkitppc:20220128
COPY --from=libiosuhax-build /out /

WORKDIR /app
CMD make -j$(nproc)
