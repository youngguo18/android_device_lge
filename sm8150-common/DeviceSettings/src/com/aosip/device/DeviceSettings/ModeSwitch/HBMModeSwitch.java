/*
* Copyright (C) 2016 The OmniROM Project
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
package com.aosip.device.DeviceSettings.ModeSwitch;

import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;

import com.aosip.device.DeviceSettings.Utils;

public class HBMModeSwitch implements OnPreferenceChangeListener{

    private static final String HBM_FILE = "/sys/devices/virtual/panel/brightness/irc_brighter";

    public static String getFile() {
        if (Utils.fileWritable(HBM_FILE)) {
            return HBM_FILE;
        }
        return null;
    }

    public static boolean isSupported() {
        return Utils.fileWritable(getFile());
    }

    public static boolean isCurrentlyEnabled() {
        return Utils.getFileValueAsBoolean(getFile(), false);
    }

    public static void setValue(String value) {
        Utils.writeValue(getFile(), value);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Boolean enabled = (Boolean) newValue;
        setValue(enabled ? "1" : "0");
        return true;
    }
}
