Define Flag (Anim);
Define Flag (Lock);

Meta:
    name:"FLYPUNCH_RIGHT"
    play-mode:"FORCE"
    clock:50
Images:
    ?ImageList(str)<"0%s.png">{351-352}:
Groups:
    @main
        ?content(int)<%s>{0-1}:
Animation:
    ?AnimationCode(str):
        "PLAY_GROUP(main, 3)"