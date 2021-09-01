FROM quarktheawesome/devkitpro-arm-ppc:r56_r39

RUN apt-get update && apt-get install -y --no-install-recommends \
  xxd \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app
CMD make -j$(nproc)
