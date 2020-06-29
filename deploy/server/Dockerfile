FROM ubuntu:18.04
ARG USER=ctf
RUN apt-get update && apt-get -y dist-upgrade
RUN apt-get install -y qemu
RUn groupadd -g 10000 $USER && useradd -N -u 10000 -g 10000 $USER
ADD --chown=root:10000 ./ /
USER 10000:10000
CMD ["/usr/bin/timeout", "-s9", "500", "/run.sh"]
