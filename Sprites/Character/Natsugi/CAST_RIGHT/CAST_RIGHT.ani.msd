Define Flag (Anim);
Define Flag (Lock);

Meta:
    name:"CAST_RIGHT"
    play-mode:"FORCE"
    clock:30
Images:
    ?ImageList(str)<"0%s.png">{423-430}:
Groups:
    @main
        ?content(int)<%s>{0-7}:
Animation:
    ?AnimationCode(str):
        "PLAY_GROUP(main, 1)"
        "CALL('IDLE_RIGHT')"