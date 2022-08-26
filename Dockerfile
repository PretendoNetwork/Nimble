FROM devkitpro/devkitppc:20220821 AS libiosuhax-build

WORKDIR /libiosuhax
RUN git init && \
    git remote add origin https://github.com/wiiu-env/libiosuhax && \
    git fetch --depth 1 origin d9bbd3e72b65b85b237709e68124f65d7ed8868c && \
    git checkout FETCH_HEAD && \
    git submodule update --init --depth 1

RUN make install DESTDIR=/out

FROM devkitpro/devkitppc:20220821
COPY --from=libiosuhax-build /out /

WORKDIR /app
CMD make -j$(nproc)
