FROM quay.io/pypa/manylinux1_x86_64

ADD setup.sh /root/setup.sh

RUN /root/setup.sh

RUN /opt/python/cp27-cp27mu/bin/pip install cget
RUN /opt/python/cp27-cp27mu/bin/pip install cmake

RUN echo -e '#!/bin/bash\n/opt/python/cp27-cp27mu/bin/cget "$@"' > /usr/bin/cget && chmod +x /usr/bin/cget
RUN echo -e '#!/bin/bash\n/opt/python/cp27-cp27mu/bin/cmake "$@"' > /usr/bin/cmake && chmod +x /usr/bin/cmake

WORKDIR /work
CMD ["echo", "#!/bin/sh\ndocker run -v $(pwd):/work -t many_x64 $@"]
