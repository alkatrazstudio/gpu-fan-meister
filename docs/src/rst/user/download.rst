Download
========

|project| can be downloaded from the official website: `<https://gpufanmeister.alkatrazstudio.net>`_. At the moment there is a build only for Linux. Also, the systems on Wayland are not supported.

|project| is distributed in the `AppImage <https://appimage.org>`_ format as a single executable file, but this file is not fully self-contained. In particular, the `Python 3 interpreter <https://www.python.org>`_ and `D-Bus <https://www.freedesktop.org/wiki/Software/dbus/>`_ must be installed to run the program on Linux. These programs are usually already pre-installed on Desktop Linux. Also, each of the :doc:`plugins <plugins>` may have its own requirements to run. For example, the requirements for the **nvidia-nvml** plugin, which is included with |project|, are described on the :doc:`corresponding page <plugins/nvidia-nvml>`.

|project| is a free and open source project, so you can try to build it from source by following :doc:`build instructions </dev/build>`.

You can view the list of changes of |project| here: `<https://github.com/alkatrazstudio/gpu-fan-meister/blob/master/CHANGELOG.rst>`_.
