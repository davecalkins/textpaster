textpaster
==========

Uses SendInput() to "paste" text from the clipboard into the current app in response to a global hotkey.  This was created in response to certain web sites which prevent you from pasting into the login password field.  This restriction makes it difficult to use a password vault app with random passwords which are then pasted into sites when logging in.

With the app running in the background, on the web site with the password field active press Ctrl+Shift+/.  After a 3 second delay (shown in the app window if its visible) the text currently on the clipboard will be "pasted" into the field (entered using SendInput() to simulate keyboard entry).
