/*
    Theengs - Decode things and devices
    Copyright: (c) Florian ROBERT

    This file is part of Theengs.

    Theengs is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Theengs is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

package com.theengs.app;

import android.content.Context;
import android.content.Intent;
import android.app.PendingIntent;
import android.app.TaskStackBuilder;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.NotificationChannel;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;

public class TheengsAndroidNotifier {

    private static String channelId = "Theengs";
    private static String channelName = "Theengs Notifier";
    private static int channelImportance = NotificationManager.IMPORTANCE_DEFAULT;

    public static void notify(Context context, String title, String message) {
        try {
            //Context context = getApplicationContext();
            NotificationManager notificationManager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
            Notification.Builder builder;

            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                NotificationChannel notificationChannel = new NotificationChannel(channelId, channelName, channelImportance);
                notificationChannel.enableLights(true);
                notificationChannel.enableVibration(false);
                notificationChannel.setLightColor(Color.GREEN);
                //notificationChannel.setVibrationPattern(new long[]{500,500,500,500,500});
                notificationChannel.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
                notificationManager.createNotificationChannel(notificationChannel);

                builder = new Notification.Builder(context, notificationChannel.getId());
            } else {
                builder = new Notification.Builder(context);
            }

            String packageName = context.getApplicationContext().getPackageName();
            Intent resultIntent = context.getPackageManager().getLaunchIntentForPackage(packageName);
            resultIntent.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
            PendingIntent resultPendingIntent = PendingIntent.getActivity(context, 0, resultIntent, PendingIntent.FLAG_UPDATE_CURRENT);

            builder.setSmallIcon(R.drawable.ic_stat_logo);
            //Bitmap icon = BitmapFactory.decodeResource(context.getResources(), R.drawable.ic_stat_logo);
            //builder.setLargeIcon(icon);
            //builder.setColor(Color.WHITE);
            builder.setContentTitle(title);
            builder.setContentText(message);
            builder.setContentIntent(resultPendingIntent);
            builder.setDefaults(Notification.DEFAULT_SOUND);
            builder.setAutoCancel(true);

            notificationManager.notify(0, builder.build());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
