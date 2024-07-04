FROM kullken/ros2_arm64v8:pet-base-humble

# Ensure all commands are run as root
USER root

# Update package lists and install a simple application
RUN apt-get update
RUN apt-get install -y \
	sudo git build-essential subversion libncurses5-dev zip unzip wget gettext texinfo chrpath cmake libboost-all-dev libcurl4-openssl-dev \
	libtar-dev libssl-dev libcurl4 valgrind vim libgtest-dev libopencv-dev

WORKDIR /usr/src/gtest
RUN cmake . && \
    make 

RUN useradd -m user \
	&& usermod -aG sudo user \
	&& echo 'user ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER user

WORKDIR /home/user

RUN git clone -b model_profile_fixes https://github.com/vsnmtej/datatracer.git
WORKDIR /home/user/datatracer/build
RUN cmake .. && \
	make

# Set a command to run when the container starts
CMD ["/bin/bash"]

