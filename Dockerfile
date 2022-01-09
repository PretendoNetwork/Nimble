FROM wiiuenv/devkitppc:20211229

RUN apt-get update && apt-get install -y --no-install-recommends \
  xxd \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app
CMD make -j$(nproc)
