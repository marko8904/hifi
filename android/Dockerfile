FROM openjdk:8

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

RUN apt-get update && apt-get -y install \
    curl \
    gnupg \
    software-properties-common \
    unzip \
    -

# --- Versions and Download paths
ENV ANDROID_HOME="/usr/local/android-sdk" \
    ANDROID_NDK_HOME="/usr/local/android-ndk" \
    ANDROID_VERSION=27 \
    ANDROID_BUILD_TOOLS_VERSION=27.0.3 \
    ANDROID_NDK_VERSION=r17 \
    GRADLE_VERSION=4.8 \
    QT_VERSION=5.10.1

ENV SDK_URL="https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip" \
    NDK_URL="https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip" \
    GRADLE_URL="https://services.gradle.org/distributions/gradle-${GRADLE_VERSION}-bin.zip"

# --- Qt
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 15FF1AAE >/dev/null
RUN add-apt-repository "deb http://debian.highfidelity.com stable main"
RUN apt-get -y update && apt-get -y install hifiqt${QT_VERSION}

ENV QT_CMAKE_PREFIX_PATH="/usr/local/Qt${QT_VERSION}/${QT_VERSION}/gcc_64/lib/cmake"

# --- Android SDK
RUN mkdir "$ANDROID_HOME" .android && \
    cd "$ANDROID_HOME" && \
    curl -s -S -o sdk.zip -L "${SDK_URL}" && \
    unzip sdk.zip && \
    rm sdk.zip && \
    yes | $ANDROID_HOME/tools/bin/sdkmanager --licenses

# Install Android Build Tool and Libraries
RUN $ANDROID_HOME/tools/bin/sdkmanager --update
RUN $ANDROID_HOME/tools/bin/sdkmanager "build-tools;${ANDROID_BUILD_TOOLS_VERSION}" \
    "platforms;android-${ANDROID_VERSION}" \
    "platform-tools"

# --- Android NDK
# download
RUN mkdir /usr/local/android-ndk-tmp && \
    cd /usr/local/android-ndk-tmp && \
    curl -s -S -o ndk.zip -L "${NDK_URL}" && \
    unzip -q ndk.zip && \
    mv ./android-ndk-${ANDROID_NDK_VERSION} ${ANDROID_NDK_HOME} && \
    cd ${ANDROID_NDK_HOME} && \
    rm -rf /usr/local/android-ndk-tmp

ENV PATH ${PATH}:${ANDROID_NDK_HOME}

# --- Gradle
WORKDIR /usr/local
RUN curl -s -S -o gradle.zip -L "${GRADLE_URL}" && \
    unzip gradle.zip && \
    rm gradle.zip

ENV PATH ${PATH}:/usr/local/gradle-${GRADLE_VERSION}/bin

RUN mkdir /hifi
WORKDIR /hifi/android
