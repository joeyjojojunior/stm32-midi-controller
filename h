[33mcommit 3d7903288453a4d5ec8312c5f3b2cffe3436064f[m[33m ([m[1;36mHEAD -> [m[1;32mmaster[m[33m)[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sat Jul 3 17:17:25 2021 -0400

    fix bug with NUM_ADC_SAMPLES=1; refactor; adc works nicely again at high clock

[33mcommit d644830e44e36d2ff5c9f99094b2780d7ce01356[m[33m ([m[1;31morigin/master[m[33m)[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sat Jul 3 16:03:53 2021 -0400

    update ioc file to reflect new ADC setup

[33mcommit 1eca6f964cc0a1cbf3b5d4b2b54f92614f992b88[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sat Jul 3 15:50:56 2021 -0400

    change from DMA to poll for ADC. much more stable at high system clocks

[33mcommit fa9df1f25aa05ea3d67ef1459e1e96e4d4072cfc[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Thu Jul 1 23:54:26 2021 -0400

    more refactoring

[33mcommit 5febdf56e923ffbf645f7d5bf9dda1621113f290[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Thu Jul 1 21:36:34 2021 -0400

    refactor ssd1306 stuff

[33mcommit acddf8985ce28e72ca486a3733c13283937353b8[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Wed Jun 30 00:32:50 2021 -0400

    some refactoring

[33mcommit 92c12a89690a39cf20acd072a565bc0d42803324[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Tue Jun 29 23:43:48 2021 -0400

    improved UI

[33mcommit 43e06d19620679275e3dae59c5a86baa125accd0[m[33m ([m[1;32mnew_map[m[33m)[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 23:43:17 2021 -0400

    top line init value indicator can now indicate percent difference between actual and init value

[33mcommit 886522cf8dc114bae5392da4e187e476055167a8[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 21:36:12 2021 -0400

    add top line indicator for initial value and locking for presets

[33mcommit 22982ff2fa15972900c109f8c8fb7721d6368ad7[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 20:00:45 2021 -0400

    can now map limited value sets to a range between 0,n, n<=127

[33mcommit f2518cf1e61f43334e1706da5b0ab82da7a931e6[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 19:48:32 2021 -0400

    sub-labels for restricted ranges sorta work; have to figure out how to remap to 0-127 if necessary

[33mcommit e434b634501be943bd295fbddf0eb25c48973193[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 18:20:12 2021 -0400

    add channel select & midi max value implementation

[33mcommit 51999439ebef88be4d164e6020eb1710bb7b911a[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 16:27:33 2021 -0400

    hard code oled addr in ssd1306 lib; move update calls within lib; cleanup

[33mcommit 508627adf8bce6404a2b07e42e386a2f34329da2[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 16:12:23 2021 -0400

    add knob struct; add function to ssd1306 lib to write a whole knob to screen

[33mcommit 2bb3459b569086dd4c62798e33eb4ab6b107fa8b[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 14:53:00 2021 -0400

    wip screen layout added

[33mcommit 1268f1e6050d98cd4f1ec47a05ee5643e7041234[m[33m ([m[1;32maveraging[m[33m, [m[1;32madafruit_ssd1306[m[33m)[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 11:52:54 2021 -0400

    cleanup

[33mcommit b7c519f28b5eb9aa6d70cf8c077c6624de696ce2[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 11:31:46 2021 -0400

    averaging + EMS works much better

[33mcommit cbd8c0eff4b9fea47f885cba1a6a5faf71ab212c[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 10:09:34 2021 -0400

    jitter minimal

[33mcommit 3308e3c8865864ba4cce4b6d0ae5917fd52ca480[m
Author: Igor Grebenkov <igor.grebenkov@gmail.com>
Date:   Sun Jun 27 02:31:45 2021 -0400

    initial commit
