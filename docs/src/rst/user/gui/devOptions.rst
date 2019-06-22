Device options
==============

Clicking on the "Device options..." button in the lower left corner of the main application window will open a dialog where you can set the settings for the video card that is currently selected. The possible settings are listed below:

* Display name
    Custom name for the video card. This name will be displayed everywhere in the interface, for example in the headings of windows, info messages, etc. If you leave this name blank, the interface will display the name of the video card, which was defined by :doc:`plugin </user/plugins>` that is responsible for interacting with this video card.


* Tray icon colors
    In this section you can set the colors that current temperature of the video card and the fan speed will be displayed in :doc:`tray <trayIcon>` .
    Here you can also set the background color of the tray icon.
    The color, which is to the right of the number, shows from which value this color will be applied.
    The color to the left of the number will be applied if the value of the corresponding indicator is less than this number.

    *Example:*

        For example, if the "Temperature" line has the following settings:

            white - 60 - yellow - 75 - red

        Then this will mean that at temperatures between 0 and 59 °C inclusive, the color of the number displaying the current tray temperature of the video card will be white.
        At temperatures between 60 and 74 °C inclusive, the color will be yellow. If the temperature is 75 °C or higher, the color will be red.

    You can change the color by clicking on it. Transparency is supported.


* Update the tray icon every N seconds
    The :doc:`tray icon <trayIcon>` will be updated no more than once in N seconds.


In addition, the "Show/Hide plugin info" button will display or hide information about the :doc:`plugin </user/plugins>` that is currently working with the selected video card.
