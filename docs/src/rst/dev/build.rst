Building from sources
=====================

|project| is available as a ready-made :doc:`build for Linux </user/download>`, but it is also possible to build everything from source manually.

Instructions for building on Linux:

1. Custom build of `Qt <https://www.qt.io>`_ with special patches and additional libraries is used. Qt's automatic build script for Linux can be found in the repository: `<https://gitlab.com/alkatrazstudio/qt5-build>`_.

2. Clone the main repository with the submodules:

    ``git clone --recursive https://github.com/alkatrazstudio/gpu-fan-meister``

3. The system must have a developer package for the `libnotify library <https://developer.gnome.org/libnotify/>`_ installed. For example, for Ubuntu, it's the **libnotify-dev** package.

4. If you want to eventually pack everything in `AppImage <https://appimage.org>`_, you need `appimagetool <https://github.com/AppImage/AppImageKit>`_ installed in your system.

5. If you also want to build a :doc:`/user/plugins/nvidia-nvml` plugin, then `NVIDIA CUDA Toolkit <https://developer.nvidia.com/cuda-toolkit>`_ must be installed in the system. For example, for Ubuntu it's **nvidia-cuda-dev** package.

6. Further instructions can be found in the ``build.sh`` file in the cloned repository.
