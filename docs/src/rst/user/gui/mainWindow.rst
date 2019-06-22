Main window
===========

In the main window of |project| you can select and configure the desired :doc:`mode <../modes>` for each video card. The window can be divided into the following parts (top-down):

.. _gui-mainWindow-mainMenu:

    * **The main menu**. Contains the following items:

        * **Application**

            * **Settings...** - open the window with general application settings.

            * **Select plugins...** - open the window ":doc:`enabledPlugins`".

            * **Exit** - exit the application.

        * **About**

            * **About Qt...** - information about the version of the Qt library used to make |project| (`<https://qt.io>`_).

            * **About plugins...** - information about currently loaded plugins. There will be no plugins in this list that were deactivated due to some error.

            * **About** |project|\ **...** - a short summary of |project|.

    * **Tabs with video card names**. Used to select a video card for which you want to select and configure its :doc:`mode <../modes>`.

    * **Mode switches**. Allow you to select the current video card :doc:`mode <../modes>`. The mode changes immediately, without pressing the "Save" button.

    * **Mode settings**. The major part of the main application window is occupied by settings of the selected mode. All changes are applied immediately, without pressing the "Save" button (:doc:`"script mode" <../modes/script>` restarts the script only on request). To cancel unsaved changes, you need to press the "Cancel" button.

    * **Buttons**:

        * **Device options...** - open the window ":doc:`devOptions`".

        * **Cancel** - Cancels all unsaved mode settings changes.

        * **Save** - saves all settings displayed in the main window for all video cards.
