EMOJI_EXCHANGE_TBL = Hash.new{|h,k| h[k] = {}}
EMOJI_EXCHANGE_TBL['UTF8-DoCoMo']['UTF8-KDDI'] = [
  ["ee98be", "ee9288"], # [BLACK SUN WITH RAYS] U+E63E -> U+E488
  ["ee98bf", "ee928d"], # [CLOUD] U+E63F -> U+E48D
  ["ee9980", "ee928c"], # [UMBRELLA WITH RAIN DROPS] U+E640 -> U+E48C
  ["ee9981", "ee9285"], # [SNOWMAN WITHOUT SNOW] U+E641 -> U+E485
  ["ee9982", "ee9287"], # [HIGH VOLTAGE SIGN] U+E642 -> U+E487
  ["ee9983", "ee91a9"], # [CYCLONE] U+E643 -> U+E469
  ["ee9984", "ee9698"], # [FOGGY] U+E644 -> U+E598
  ["ee9985", "eeaba8"], # [CLOSED UMBRELLA] U+E645 -> U+EAE8
  ["ee9ab3", "eeabb1"], # [NIGHT WITH STARS] U+E6B3 -> U+EAF1
  ["ee9cbf", "eeadbc"], # [WATER WAVE] U+E73F -> U+EB7C
  ["ee9a9c", "ee96a8"], # [NEW MOON SYMBOL] U+E69C -> U+E5A8
  ["ee9a9d", "ee96a9"], # [WAXING GIBBOUS MOON SYMBOL] U+E69D -> U+E5A9
  ["ee9a9e", "ee96aa"], # [FIRST QUARTER MOON SYMBOL] U+E69E -> U+E5AA
  ["ee9a9f", "ee9286"], # [CRESCENT MOON] U+E69F -> U+E486
  ["ee9aa0", :undef], # [FULL MOON SYMBOL] U+E6A0 -> "○"
  ["ee9c9f", "ee95ba"], # [WATCH] U+E71F -> U+E57A
  ["ee9aba", "ee9694"], # [ALARM CLOCK] U+E6BA -> U+E594
  ["ee9c9c", "ee91bc"], # [HOURGLASS WITH FLOWING SAND] U+E71C -> U+E47C
  ["ee9986", "ee928f"], # [ARIES] U+E646 -> U+E48F
  ["ee9987", "ee9290"], # [TAURUS] U+E647 -> U+E490
  ["ee9988", "ee9291"], # [GEMINI] U+E648 -> U+E491
  ["ee9989", "ee9292"], # [CANCER] U+E649 -> U+E492
  ["ee998a", "ee9293"], # [LEO] U+E64A -> U+E493
  ["ee998b", "ee9294"], # [VIRGO] U+E64B -> U+E494
  ["ee998c", "ee9295"], # [LIBRA] U+E64C -> U+E495
  ["ee998d", "ee9296"], # [SCORPIUS] U+E64D -> U+E496
  ["ee998e", "ee9297"], # [SAGITTARIUS] U+E64E -> U+E497
  ["ee998f", "ee9298"], # [CAPRICORN] U+E64F -> U+E498
  ["ee9990", "ee9299"], # [AQUARIUS] U+E650 -> U+E499
  ["ee9991", "ee929a"], # [PISCES] U+E651 -> U+E49A
  ["ee9d81", "ee9493"], # [FOUR LEAF CLOVER] U+E741 -> U+E513
  ["ee9d83", "ee93a4"], # [TULIP] U+E743 -> U+E4E4
  ["ee9d86", "eeadbd"], # [SEEDLING] U+E746 -> U+EB7D
  ["ee9d87", "ee938e"], # [MAPLE LEAF] U+E747 -> U+E4CE
  ["ee9d88", "ee938a"], # [CHERRY BLOSSOM] U+E748 -> U+E4CA
  ["ee9d82", "ee9392"], # [CHERRIES] U+E742 -> U+E4D2
  ["ee9d84", "eeacb5"], # [BANANA] U+E744 -> U+EB35
  ["ee9d85", "eeaab9"], # [RED APPLE] U+E745 -> U+EAB9
  ["ee9a91", "ee96a4"], # [EYES] U+E691 -> U+E5A4
  ["ee9a92", "ee96a5"], # [EAR] U+E692 -> U+E5A5
  ["ee9c90", "ee9489"], # [LIPSTICK] U+E710 -> U+E509
  ["ee9ab1", :undef], # [BUST IN SILHOUETTE] U+E6B1 -> U+3013 (GETA)
  ["ee9d8e", "eeadbe"], # [SNAIL] U+E74E -> U+EB7E
  ["ee9d8f", "ee93a0"], # [BABY CHICK] U+E74F -> U+E4E0
  ["ee9d90", "ee939c"], # [PENGUIN] U+E750 -> U+E4DC
  ["ee9d91", "ee929a"], # [FISH] U+E751 -> U+E49A
  ["ee9aa2", "ee939b"], # [CAT FACE] U+E6A2 -> U+E4DB
  ["ee9d94", "ee9398"], # [HORSE FACE] U+E754 -> U+E4D8
  ["ee9aa1", "ee93a1"], # [DOG FACE] U+E6A1 -> U+E4E1
  ["ee9d95", "ee939e"], # [PIG FACE] U+E755 -> U+E4DE
  ["ee9bb1", "ee91b2"], # [ANGRY FACE] U+E6F1 -> U+E472
  ["ee9bb2", "eeab80"], # [DISAPPOINTED FACE] U+E6F2 -> U+EAC0
  ["ee9bb4", "ee96ae"], # [DIZZY FACE] U+E6F4 -> U+E5AE
  ["ee9ca5", "eeab89"], # [EXPRESSIONLESS FACE] U+E725 -> U+EAC9
  ["ee9ca6", "ee9784"], # [FACE WITH HEART-SHAPED EYES] U+E726 -> U+E5C4
  ["ee9ca8", "ee93a7"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E728 -> U+E4E7
  ["ee9d92", "eeab8d"], # [FACE SAVOURING DELICIOUS FOOD] U+E752 -> U+EACD
  ["ee9bb0", "ee91b1"], # [HAPPY FACE WITH OPEN MOUTH] U+E6F0 -> U+E471
  ["ee9ca2", "ee91b1ee96b1"], # [HAPPY FACE WITH OPEN MOUTH AND COLD SWEAT] U+E722 -> U+E471 U+E5B1
  ["ee9caa", "eeab85"], # [HAPPY FACE WITH OPEN MOUTH AND CLOSED EYES] U+E72A -> U+EAC5
  ["ee9d93", "eeae80"], # [HAPPY FACE WITH GRIN] U+E753 -> U+EB80
  ["ee9cae", "eeada9"], # [CRYING FACE] U+E72E -> U+EB69
  ["ee9cad", "ee91b3"], # [LOUDLY CRYING FACE] U+E72D -> U+E473
  ["ee9cab", "eeab82"], # [PERSEVERING FACE] U+E72B -> U+EAC2
  ["ee9ca4", "eead9d"], # [POUTING FACE] U+E724 -> U+EB5D
  ["ee9ca1", "eeab85"], # [RELIEVED FACE] U+E721 -> U+EAC5
  ["ee9bb3", "eeab83"], # [CONFOUNDED FACE] U+E6F3 -> U+EAC3
  ["ee9ca0", "eeab80"], # [PENSIVE FACE] U+E720 -> U+EAC0
  ["ee9d97", "ee9785"], # [FACE SCREAMING IN FEAR] U+E757 -> U+E5C5
  ["ee9cac", "eeaabf"], # [SMIRKING FACE] U+E72C -> U+EABF
  ["ee9ca3", "ee9786"], # [FACE WITH COLD SWEAT] U+E723 -> U+E5C6
  ["ee9ca9", "ee9783"], # [WINKING FACE] U+E729 -> U+E5C3
  ["ee99a3", "ee92ab"], # [HOUSE BUILDING] U+E663 -> U+E4AB
  ["ee99a4", "ee92ad"], # [OFFICE BUILDING] U+E664 -> U+E4AD
  ["ee99a5", "ee979e"], # [JAPANESE POST OFFICE] U+E665 -> U+E5DE
  ["ee99a6", "ee979f"], # [HOSPITAL] U+E666 -> U+E5DF
  ["ee99a7", "ee92aa"], # [BANK] U+E667 -> U+E4AA
  ["ee99a8", "ee92a3"], # [AUTOMATED TELLER MACHINE] U+E668 -> U+E4A3
  ["ee99a9", "eeaa81"], # [HOTEL] U+E669 -> U+EA81
  ["ee99aa", "ee92a4"], # [CONVENIENCE STORE] U+E66A -> U+E4A4
  ["ee9cbe", "eeaa80"], # [SCHOOL] U+E73E -> U+EA80
  ["ee9d80", "ee96bd"], # [MOUNT FUJI] U+E740 -> U+E5BD
  ["ee9a99", "eeacab"], # [ATHLETIC SHOE] U+E699 -> U+EB2B
  ["ee99b4", "ee949a"], # [HIGH-HEELED SHOE] U+E674 -> U+E51A
  ["ee9a98", "eeacaa"], # [FOOTPRINTS] U+E698 -> U+EB2A
  ["ee9a9a", "ee93be"], # [EYEGLASSES] U+E69A -> U+E4FE
  ["ee9c8e", "ee96b6"], # [T-SHIRT] U+E70E -> U+E5B6
  ["ee9c91", "eeadb7"], # [JEANS] U+E711 -> U+EB77
  ["ee9c9a", "ee9789"], # [CROWN] U+E71A -> U+E5C9
  ["ee9c8f", "ee9484"], # [PURSE] U+E70F -> U+E504
  ["ee9a82", "ee929c"], # [HANDBAG] U+E682 -> U+E49C
  ["ee9aad", :undef], # [POUCH] U+E6AD -> "[ふくろ]"
  ["ee9c95", "ee9387"], # [MONEY BAG] U+E715 -> U+E4C7
  ["ee9b96", "ee95bd"], # [BANKNOTE WITH YEN SIGN] U+E6D6 -> U+E57D
  ["ee9c98", "ee9687"], # [WRENCH] U+E718 -> U+E587
  ["ee9a84", "ee969f"], # [RIBBON] U+E684 -> U+E59F
  ["ee9a85", "ee938f"], # [WRAPPED PRESENT] U+E685 -> U+E4CF
  ["ee9a86", "ee96a0"], # [BIRTHDAY CAKE] U+E686 -> U+E5A0
  ["ee9aa4", "ee9389"], # [CHRISTMAS TREE] U+E6A4 -> U+E4C9
  ["ee999a", "ee969b"], # [PAGER] U+E65A -> U+E59B
  ["ee9a87", "ee9696"], # [BLACK TELEPHONE] U+E687 -> U+E596
  ["ee9a88", "ee9688"], # [MOBILE PHONE] U+E688 -> U+E588
  ["ee9b8e", "eeac88"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+E6CE -> U+EB08
  ["ee9a89", "eeaa92"], # [MEMO] U+E689 -> U+EA92
  ["ee9b90", "ee94a0"], # [FAX MACHINE] U+E6D0 -> U+E520
  ["ee9b93", "ee94a1"], # [ENVELOPE] U+E6D3 -> U+E521
  ["ee9b8f", "eeada2"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+E6CF -> U+EB62
  ["ee9aae", "eeac83"], # [BLACK NIB] U+E6AE -> U+EB03
  ["ee9ab2", :undef], # [SEAT] U+E6B2 -> "[いす]"
  ["ee9c96", "ee96b8"], # [PERSONAL COMPUTER] U+E716 -> U+E5B8
  ["ee9c99", "ee92a1"], # [PENCIL] U+E719 -> U+E4A1
  ["ee9cb0", "ee92a0"], # [PAPERCLIP] U+E730 -> U+E4A0
  ["ee9a8c", "ee948c"], # [OPTICAL DISC] U+E68C -> U+E50C
  ["ee99b5", "ee9496"], # [BLACK SCISSORS] U+E675 -> U+E516
  ["ee9a83", "ee929f"], # [OPEN BOOK] U+E683 -> U+E49F
  ["ee9992", :undef], # [RUNNING SHIRT WITH SASH] U+E652 -> U+3013 (GETA)
  ["ee9993", "ee92ba"], # [BASEBALL] U+E653 -> U+E4BA
  ["ee9994", "ee9699"], # [FLAG IN HOLE] U+E654 -> U+E599
  ["ee9995", "ee92b7"], # [TENNIS RACQUET AND BALL] U+E655 -> U+E4B7
  ["ee9996", "ee92b6"], # [SOCCER BALL] U+E656 -> U+E4B6
  ["ee9997", "eeaaac"], # [SKI AND SKI BOOT] U+E657 -> U+EAAC
  ["ee9998", "ee969a"], # [BASKETBALL AND HOOP] U+E658 -> U+E59A
  ["ee9999", "ee92b9"], # [CHEQUERED FLAG] U+E659 -> U+E4B9
  ["ee9c92", "ee92b8"], # [SNOWBOARDER] U+E712 -> U+E4B8
  ["ee9cb3", "ee91ab"], # [RUNNER] U+E733 -> U+E46B
  ["ee999b", "ee92b5"], # [TRAIN] U+E65B -> U+E4B5
  ["ee999c", "ee96bc"], # [CIRCLED LATIN CAPITAL LETTER M] U+E65C -> U+E5BC
  ["ee999d", "ee92b0"], # [HIGH-SPEED TRAIN] U+E65D -> U+E4B0
  ["ee999e", "ee92b1"], # [AUTOMOBILE] U+E65E -> U+E4B1
  ["ee999f", "ee92b1"], # [RECREATIONAL VEHICLE] U+E65F -> U+E4B1
  ["ee99a0", "ee92af"], # [ONCOMING BUS] U+E660 -> U+E4AF
  ["ee99a1", "eeaa82"], # [SHIP] U+E661 -> U+EA82
  ["ee99a2", "ee92b3"], # [AIRPLANE] U+E662 -> U+E4B3
  ["ee9aa3", "ee92b4"], # [SAILBOAT] U+E6A3 -> U+E4B4
  ["ee99ab", "ee95b1"], # [FUEL PUMP] U+E66B -> U+E571
  ["ee99ac", "ee92a6"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E66C -> U+E4A6
  ["ee99ad", "ee91aa"], # [HORIZONTAL TRAFFIC LIGHT] U+E66D -> U+E46A
  ["ee9bb7", "ee92bc"], # [HOT SPRINGS] U+E6F7 -> U+E4BC
  ["ee99b9", :undef], # [CAROUSEL HORSE] U+E679 -> U+3013 (GETA)
  ["ee99b6", "ee9483"], # [MICROPHONE] U+E676 -> U+E503
  ["ee99b7", "ee9497"], # [MOVIE CAMERA] U+E677 -> U+E517
  ["ee99ba", "ee9488"], # [HEADPHONE] U+E67A -> U+E508
  ["ee99bb", "ee969c"], # [ARTIST PALETTE] U+E67B -> U+E59C
  ["ee99bc", "eeabb5"], # [TOP HAT] U+E67C -> U+EAF5
  ["ee99bd", "ee969e"], # [CIRCUS TENT] U+E67D -> U+E59E
  ["ee99be", "ee929e"], # [TICKET] U+E67E -> U+E49E
  ["ee9aac", "ee92be"], # [CLAPPER BOARD] U+E6AC -> U+E4BE
  ["ee9a8b", "ee9386"], # [VIDEO GAME] U+E68B -> U+E4C6
  ["ee9bb6", "ee96be"], # [MUSICAL NOTE] U+E6F6 -> U+E5BE
  ["ee9bbf", "ee9485"], # [MULTIPLE MUSICAL NOTES] U+E6FF -> U+E505
  ["ee9a81", "ee9495"], # [CAMERA] U+E681 -> U+E515
  ["ee9a8a", "ee9482"], # [TELEVISION] U+E68A -> U+E502
  ["ee9bb9", "ee93ab"], # [KISS MARK] U+E6F9 -> U+E4EB
  ["ee9c97", "eeadb8"], # [LOVE LETTER] U+E717 -> U+EB78
  ["ee9c9b", "ee9494"], # [RING] U+E71B -> U+E514
  ["ee9cb1", "ee9598"], # [COPYRIGHT SIGN] U+E731 -> U+E558
  ["ee9cb6", "ee9599"], # [REGISTERED SIGN] U+E736 -> U+E559
  ["ee9cb2", "ee958e"], # [TRADE MARK SIGN] U+E732 -> U+E54E
  ["ee9ba0", "eeae84"], # [HASH KEY] U+E6E0 -> U+EB84
  ["ee9ba2", "ee94a2"], # [KEYCAP 1] U+E6E2 -> U+E522
  ["ee9ba3", "ee94a3"], # [KEYCAP 2] U+E6E3 -> U+E523
  ["ee9ba4", "ee94a4"], # [KEYCAP 3] U+E6E4 -> U+E524
  ["ee9ba5", "ee94a5"], # [KEYCAP 4] U+E6E5 -> U+E525
  ["ee9ba6", "ee94a6"], # [KEYCAP 5] U+E6E6 -> U+E526
  ["ee9ba7", "ee94a7"], # [KEYCAP 6] U+E6E7 -> U+E527
  ["ee9ba8", "ee94a8"], # [KEYCAP 7] U+E6E8 -> U+E528
  ["ee9ba9", "ee94a9"], # [KEYCAP 8] U+E6E9 -> U+E529
  ["ee9baa", "ee94aa"], # [KEYCAP 9] U+E6EA -> U+E52A
  ["ee9bab", "ee96ac"], # [KEYCAP 0] U+E6EB -> U+E5AC
  ["ee99b3", "ee9396"], # [HAMBURGER] U+E673 -> U+E4D6
  ["ee9d89", "ee9395"], # [RICE BALL] U+E749 -> U+E4D5
  ["ee9d8a", "ee9390"], # [SHORTCAKE] U+E74A -> U+E4D0
  ["ee9d8c", "ee96b4"], # [STEAMING BOWL] U+E74C -> U+E5B4
  ["ee9d8d", "eeaaaf"], # [BREAD] U+E74D -> U+EAAF
  ["ee99af", "ee92ac"], # [FORK AND KNIFE] U+E66F -> U+E4AC
  ["ee99b0", "ee9697"], # [HOT BEVERAGE] U+E670 -> U+E597
  ["ee99b1", "ee9382"], # [COCKTAIL GLASS] U+E671 -> U+E4C2
  ["ee99b2", "ee9383"], # [BEER MUG] U+E672 -> U+E4C3
  ["ee9c9e", "eeaaae"], # [TEACUP WITHOUT HANDLE] U+E71E -> U+EAAE
  ["ee9d8b", "eeaa97"], # [SAKE BOTTLE AND CUP] U+E74B -> U+EA97
  ["ee9d96", "ee9381"], # [WINE GLASS] U+E756 -> U+E4C1
  ["ee99b8", "ee9595"], # [NORTH EAST ARROW] U+E678 -> U+E555
  ["ee9a96", "ee958d"], # [SOUTH EAST ARROW] U+E696 -> U+E54D
  ["ee9a97", "ee958c"], # [NORTH WEST ARROW] U+E697 -> U+E54C
  ["ee9aa5", "ee9596"], # [SOUTH WEST ARROW] U+E6A5 -> U+E556
  ["ee9bb5", "eeacad"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+E6F5 -> U+EB2D
  ["ee9c80", "eeacae"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+E700 -> U+EB2E
  ["ee9cbc", "eeadba"], # [LEFT RIGHT ARROW] U+E73C -> U+EB7A
  ["ee9cbd", "eeadbb"], # [UP DOWN ARROW] U+E73D -> U+EB7B
  ["ee9c82", "ee9282"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E702 -> U+E482
  ["ee9c83", "eeacaf"], # [EXCLAMATION QUESTION MARK] U+E703 -> U+EB2F
  ["ee9c84", "eeacb0"], # [DOUBLE EXCLAMATION MARK] U+E704 -> U+EB30
  ["ee9c89", :undef], # [WAVY DASH] U+E709 -> U+3013 (GETA)
  ["ee9c8a", "eeacb1"], # [CURLY LOOP] U+E70A -> U+EB31
  ["ee9b9f", :undef], # [DOUBLE CURLY LOOP] U+E6DF -> "[フリーダイヤル]"
  ["ee9bac", "ee9695"], # [HEAVY BLACK HEART] U+E6EC -> U+E595
  ["ee9bad", "eeadb5"], # [BEATING HEART] U+E6ED -> U+EB75
  ["ee9bae", "ee91b7"], # [BROKEN HEART] U+E6EE -> U+E477
  ["ee9baf", "ee91b8"], # [TWO HEARTS] U+E6EF -> U+E478
  ["ee9a8d", "eeaaa5"], # [BLACK HEART SUIT] U+E68D -> U+EAA5
  ["ee9a8e", "ee96a1"], # [BLACK SPADE SUIT] U+E68E -> U+E5A1
  ["ee9a8f", "ee96a2"], # [BLACK DIAMOND SUIT] U+E68F -> U+E5A2
  ["ee9a90", "ee96a3"], # [BLACK CLUB SUIT] U+E690 -> U+E5A3
  ["ee99bf", "ee91bd"], # [SMOKING SYMBOL] U+E67F -> U+E47D
  ["ee9a80", "ee91be"], # [NO SMOKING SYMBOL] U+E680 -> U+E47E
  ["ee9a9b", "ee91bf"], # [WHEELCHAIR SYMBOL] U+E69B -> U+E47F
  ["ee9b9e", "eeacac"], # [TRIANGULAR FLAG ON POST] U+E6DE -> U+EB2C
  ["ee9cb7", "ee9281"], # [WARNING SIGN] U+E737 -> U+E481
  ["ee9cb5", "eeadb9"], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+E735 -> U+EB79
  ["ee9c9d", "ee92ae"], # [BICYCLE] U+E71D -> U+E4AE
  ["ee99ae", "ee92a5"], # [RESTROOM] U+E66E -> U+E4A5
  ["ee9c94", :undef], # [DOOR] U+E714 -> "[ドア]"
  ["ee9b9b", "ee96ab"], # [SQUARED CL] U+E6DB -> U+E5AB
  ["ee9b97", "ee95b8"], # [SQUARED FREE] U+E6D7 -> U+E578
  ["ee9b98", "eeaa88"], # [SQUARED ID] U+E6D8 -> U+EA88
  ["ee9b9d", "ee96b5"], # [SQUARED NEW] U+E6DD -> U+E5B5
  ["ee9caf", :undef], # [SQUARED NG] U+E72F -> "[NG]"
  ["ee9c8b", "ee96ad"], # [SQUARED OK] U+E70B -> U+E5AD
  ["ee9cb8", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7981] U+E738 -> "[禁]"
  ["ee9cb9", "eeaa8a"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+E739 -> U+EA8A
  ["ee9cba", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5408] U+E73A -> "[合]"
  ["ee9cbb", "eeaa89"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+E73B -> U+EA89
  ["ee9cb4", "ee93b1"], # [CIRCLED IDEOGRAPH SECRET] U+E734 -> U+E4F1
  ["ee9bb8", :undef], # [DIAMOND SHAPE WITH A DOT INSIDE] U+E6F8 -> U+3013 (GETA)
  ["ee9bbb", "ee91b6"], # [ELECTRIC LIGHT BULB] U+E6FB -> U+E476
  ["ee9bbc", "ee93a5"], # [ANGER SYMBOL] U+E6FC -> U+E4E5
  ["ee9bbe", "ee91ba"], # [BOMB] U+E6FE -> U+E47A
  ["ee9c81", "ee91b5"], # [SLEEPING SYMBOL] U+E701 -> U+E475
  ["ee9c85", "ee96b0"], # [COLLISION SYMBOL] U+E705 -> U+E5B0
  ["ee9c86", "ee96b1"], # [SPLASHING SWEAT SYMBOL] U+E706 -> U+E5B1
  ["ee9c87", "ee93a6"], # [DROP OF WATER] U+E707 -> U+E4E6
  ["ee9c88", "ee93b4"], # [DASH SYMBOL] U+E708 -> U+E4F4
  ["ee9bba", "eeaaab"], # [SPARKLES] U+E6FA -> U+EAAB
  ["ee9b9a", "ee959d"], # [LEFTWARDS ARROW WITH HOOK] U+E6DA -> U+E55D
  ["ee9b9c", "ee9498"], # [LEFT-POINTING MAGNIFYING GLASS] U+E6DC -> U+E518
  ["ee9b99", "ee9499"], # [KEY] U+E6D9 -> U+E519
  ["ee9c93", "ee9492"], # [BELL] U+E713 -> U+E512
  ["ee9ab9", :undef], # [END WITH LEFTWARDS ARROW ABOVE] U+E6B9 -> "[end]"
  ["ee9ab8", :undef], # [ON WITH EXCLAMATION MARK WITH LEFT RIGHT ARROW ABOVE] U+E6B8 -> "[ON]"
  ["ee9ab7", :undef], # [SOON WITH RIGHTWARDS ARROW ABOVE] U+E6B7 -> "[SOON]"
  ["ee9a93", "eeae83"], # [RAISED FIST] U+E693 -> U+EB83
  ["ee9a95", "ee96a7"], # [RAISED HAND] U+E695 -> U+E5A7
  ["ee9a94", "ee96a6"], # [VICTORY HAND] U+E694 -> U+E5A6
  ["ee9bbd", "ee93b3"], # [FISTED HAND SIGN] U+E6FD -> U+E4F3
  ["ee9ca7", "ee93b9"], # [THUMBS UP SIGN] U+E727 -> U+E4F9
  ["ee9b91", :undef], # [EMOJI COMPATIBILITY SYMBOL-1] U+E6D1 -> "[iモード]"
  ["ee9b92", :undef], # [EMOJI COMPATIBILITY SYMBOL-2] U+E6D2 -> "[iモード]"
  ["ee9b94", :undef], # [EMOJI COMPATIBILITY SYMBOL-3] U+E6D4 -> "[ドコモ]"
  ["ee9b95", :undef], # [EMOJI COMPATIBILITY SYMBOL-4] U+E6D5 -> "[ドコモポイント]"
  ["ee9c8c", :undef], # [EMOJI COMPATIBILITY SYMBOL-5] U+E70C -> "[iアプリ]"
  ["ee9c8d", :undef], # [EMOJI COMPATIBILITY SYMBOL-6] U+E70D -> "[iアプリ]"
  ["ee9aa6", :undef], # [EMOJI COMPATIBILITY SYMBOL-7] U+E6A6 -> "[ぴ]"
  ["ee9aa7", :undef], # [EMOJI COMPATIBILITY SYMBOL-8] U+E6A7 -> "[あ]"
  ["ee9aa8", :undef], # [EMOJI COMPATIBILITY SYMBOL-9] U+E6A8 -> "[チケット]"
  ["ee9aa9", :undef], # [EMOJI COMPATIBILITY SYMBOL-10] U+E6A9 -> "[チケット]"
  ["ee9aaa", :undef], # [EMOJI COMPATIBILITY SYMBOL-11] U+E6AA -> "[電話先行]"
  ["ee9aab", :undef], # [EMOJI COMPATIBILITY SYMBOL-12] U+E6AB -> "[Pコード]"
  ["ee9aaf", "ee9497"], # [EMOJI COMPATIBILITY SYMBOL-13] U+E6AF -> U+E517
  ["ee9ab0", :undef], # [EMOJI COMPATIBILITY SYMBOL-14] U+E6B0 -> "[ぴ]"
  ["ee9ab4", :undef], # [EMOJI COMPATIBILITY SYMBOL-15] U+E6B4 -> "(ぴ)"
  ["ee9ab5", :undef], # [EMOJI COMPATIBILITY SYMBOL-16] U+E6B5 -> "[ぴ]"
  ["ee9ab6", :undef], # [EMOJI COMPATIBILITY SYMBOL-17] U+E6B6 -> "[チェック]"
  ["ee9abb", :undef], # [EMOJI COMPATIBILITY SYMBOL-18] U+E6BB -> "[F]"
  ["ee9abc", :undef], # [EMOJI COMPATIBILITY SYMBOL-19] U+E6BC -> "[D]"
  ["ee9abd", :undef], # [EMOJI COMPATIBILITY SYMBOL-20] U+E6BD -> "[S]"
  ["ee9abe", :undef], # [EMOJI COMPATIBILITY SYMBOL-21] U+E6BE -> "[C]"
  ["ee9abf", :undef], # [EMOJI COMPATIBILITY SYMBOL-22] U+E6BF -> "[R]"
  ["ee9b80", :undef], # [EMOJI COMPATIBILITY SYMBOL-23] U+E6C0 -> "■"
  ["ee9b81", :undef], # [EMOJI COMPATIBILITY SYMBOL-24] U+E6C1 -> "■"
  ["ee9b82", :undef], # [EMOJI COMPATIBILITY SYMBOL-25] U+E6C2 -> "▼"
  ["ee9b83", :undef], # [EMOJI COMPATIBILITY SYMBOL-26] U+E6C3 -> "††††"
  ["ee9b84", :undef], # [EMOJI COMPATIBILITY SYMBOL-27] U+E6C4 -> "†††"
  ["ee9b85", :undef], # [EMOJI COMPATIBILITY SYMBOL-28] U+E6C5 -> "††"
  ["ee9b86", :undef], # [EMOJI COMPATIBILITY SYMBOL-29] U+E6C6 -> "†"
  ["ee9b87", :undef], # [EMOJI COMPATIBILITY SYMBOL-30] U+E6C7 -> "[I]"
  ["ee9b88", :undef], # [EMOJI COMPATIBILITY SYMBOL-31] U+E6C8 -> "[M]"
  ["ee9b89", :undef], # [EMOJI COMPATIBILITY SYMBOL-32] U+E6C9 -> "[E]"
  ["ee9b8a", :undef], # [EMOJI COMPATIBILITY SYMBOL-33] U+E6CA -> "[VE]"
  ["ee9b8b", :undef], # [EMOJI COMPATIBILITY SYMBOL-34] U+E6CB -> "●"
  ["ee9b8c", :undef], # [EMOJI COMPATIBILITY SYMBOL-35] U+E6CC -> "[カード使用不可]"
  ["ee9b8d", "eeac82"], # [EMOJI COMPATIBILITY SYMBOL-36] U+E6CD -> U+EB02
  ["ee9ba1", "ee94ac"], # [EMOJI COMPATIBILITY SYMBOL-66] U+E6E1 -> U+E52C
]

EMOJI_EXCHANGE_TBL['UTF8-DoCoMo']['UTF8-SoftBank'] = [
  ["ee98be", "ee818a"], # [BLACK SUN WITH RAYS] U+E63E -> U+E04A
  ["ee98bf", "ee8189"], # [CLOUD] U+E63F -> U+E049
  ["ee9980", "ee818b"], # [UMBRELLA WITH RAIN DROPS] U+E640 -> U+E04B
  ["ee9981", "ee8188"], # [SNOWMAN WITHOUT SNOW] U+E641 -> U+E048
  ["ee9982", "ee84bd"], # [HIGH VOLTAGE SIGN] U+E642 -> U+E13D
  ["ee9983", "ee9183"], # [CYCLONE] U+E643 -> U+E443
  ["ee9984", :undef], # [FOGGY] U+E644 -> "[霧]"
  ["ee9985", "ee90bc"], # [CLOSED UMBRELLA] U+E645 -> U+E43C
  ["ee9ab3", "ee918b"], # [NIGHT WITH STARS] U+E6B3 -> U+E44B
  ["ee9cbf", "ee90be"], # [WATER WAVE] U+E73F -> U+E43E
  ["ee9a9c", :undef], # [NEW MOON SYMBOL] U+E69C -> "●"
  ["ee9a9d", "ee818c"], # [WAXING GIBBOUS MOON SYMBOL] U+E69D -> U+E04C
  ["ee9a9e", "ee818c"], # [FIRST QUARTER MOON SYMBOL] U+E69E -> U+E04C
  ["ee9a9f", "ee818c"], # [CRESCENT MOON] U+E69F -> U+E04C
  ["ee9aa0", :undef], # [FULL MOON SYMBOL] U+E6A0 -> "○"
  ["ee9c9f", :undef], # [WATCH] U+E71F -> "[腕時計]"
  ["ee9aba", "ee80ad"], # [ALARM CLOCK] U+E6BA -> U+E02D
  ["ee9c9c", :undef], # [HOURGLASS WITH FLOWING SAND] U+E71C -> "[砂時計]"
  ["ee9986", "ee88bf"], # [ARIES] U+E646 -> U+E23F
  ["ee9987", "ee8980"], # [TAURUS] U+E647 -> U+E240
  ["ee9988", "ee8981"], # [GEMINI] U+E648 -> U+E241
  ["ee9989", "ee8982"], # [CANCER] U+E649 -> U+E242
  ["ee998a", "ee8983"], # [LEO] U+E64A -> U+E243
  ["ee998b", "ee8984"], # [VIRGO] U+E64B -> U+E244
  ["ee998c", "ee8985"], # [LIBRA] U+E64C -> U+E245
  ["ee998d", "ee8986"], # [SCORPIUS] U+E64D -> U+E246
  ["ee998e", "ee8987"], # [SAGITTARIUS] U+E64E -> U+E247
  ["ee998f", "ee8988"], # [CAPRICORN] U+E64F -> U+E248
  ["ee9990", "ee8989"], # [AQUARIUS] U+E650 -> U+E249
  ["ee9991", "ee898a"], # [PISCES] U+E651 -> U+E24A
  ["ee9d81", "ee8490"], # [FOUR LEAF CLOVER] U+E741 -> U+E110
  ["ee9d83", "ee8c84"], # [TULIP] U+E743 -> U+E304
  ["ee9d86", "ee8490"], # [SEEDLING] U+E746 -> U+E110
  ["ee9d87", "ee8498"], # [MAPLE LEAF] U+E747 -> U+E118
  ["ee9d88", "ee80b0"], # [CHERRY BLOSSOM] U+E748 -> U+E030
  ["ee9d82", :undef], # [CHERRIES] U+E742 -> "[さくらんぼ]"
  ["ee9d84", :undef], # [BANANA] U+E744 -> "[バナナ]"
  ["ee9d85", "ee8d85"], # [RED APPLE] U+E745 -> U+E345
  ["ee9a91", "ee9099"], # [EYES] U+E691 -> U+E419
  ["ee9a92", "ee909b"], # [EAR] U+E692 -> U+E41B
  ["ee9c90", "ee8c9c"], # [LIPSTICK] U+E710 -> U+E31C
  ["ee9ab1", :undef], # [BUST IN SILHOUETTE] U+E6B1 -> U+3013 (GETA)
  ["ee9d8e", :undef], # [SNAIL] U+E74E -> "[カタツムリ]"
  ["ee9d8f", "ee94a3"], # [BABY CHICK] U+E74F -> U+E523
  ["ee9d90", "ee8195"], # [PENGUIN] U+E750 -> U+E055
  ["ee9d91", "ee8099"], # [FISH] U+E751 -> U+E019
  ["ee9aa2", "ee818f"], # [CAT FACE] U+E6A2 -> U+E04F
  ["ee9d94", "ee809a"], # [HORSE FACE] U+E754 -> U+E01A
  ["ee9aa1", "ee8192"], # [DOG FACE] U+E6A1 -> U+E052
  ["ee9d95", "ee848b"], # [PIG FACE] U+E755 -> U+E10B
  ["ee9bb1", "ee8199"], # [ANGRY FACE] U+E6F1 -> U+E059
  ["ee9bb2", "ee8198"], # [DISAPPOINTED FACE] U+E6F2 -> U+E058
  ["ee9bb4", "ee9086"], # [DIZZY FACE] U+E6F4 -> U+E406
  ["ee9ca5", "ee908e"], # [EXPRESSIONLESS FACE] U+E725 -> U+E40E
  ["ee9ca6", "ee8486"], # [FACE WITH HEART-SHAPED EYES] U+E726 -> U+E106
  ["ee9ca8", "ee8485"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E728 -> U+E105
  ["ee9d92", "ee8196"], # [FACE SAVOURING DELICIOUS FOOD] U+E752 -> U+E056
  ["ee9bb0", "ee8197"], # [HAPPY FACE WITH OPEN MOUTH] U+E6F0 -> U+E057
  ["ee9ca2", "ee9095ee8cb1"], # [HAPPY FACE WITH OPEN MOUTH AND COLD SWEAT] U+E722 -> U+E415 U+E331
  ["ee9caa", "ee908a"], # [HAPPY FACE WITH OPEN MOUTH AND CLOSED EYES] U+E72A -> U+E40A
  ["ee9d93", "ee9084"], # [HAPPY FACE WITH GRIN] U+E753 -> U+E404
  ["ee9cae", "ee9093"], # [CRYING FACE] U+E72E -> U+E413
  ["ee9cad", "ee9091"], # [LOUDLY CRYING FACE] U+E72D -> U+E411
  ["ee9cab", "ee9086"], # [PERSEVERING FACE] U+E72B -> U+E406
  ["ee9ca4", "ee9096"], # [POUTING FACE] U+E724 -> U+E416
  ["ee9ca1", "ee908a"], # [RELIEVED FACE] U+E721 -> U+E40A
  ["ee9bb3", "ee9087"], # [CONFOUNDED FACE] U+E6F3 -> U+E407
  ["ee9ca0", "ee9083"], # [PENSIVE FACE] U+E720 -> U+E403
  ["ee9d97", "ee8487"], # [FACE SCREAMING IN FEAR] U+E757 -> U+E107
  ["ee9cac", "ee9082"], # [SMIRKING FACE] U+E72C -> U+E402
  ["ee9ca3", "ee8488"], # [FACE WITH COLD SWEAT] U+E723 -> U+E108
  ["ee9ca9", "ee9085"], # [WINKING FACE] U+E729 -> U+E405
  ["ee99a3", "ee80b6"], # [HOUSE BUILDING] U+E663 -> U+E036
  ["ee99a4", "ee80b8"], # [OFFICE BUILDING] U+E664 -> U+E038
  ["ee99a5", "ee8593"], # [JAPANESE POST OFFICE] U+E665 -> U+E153
  ["ee99a6", "ee8595"], # [HOSPITAL] U+E666 -> U+E155
  ["ee99a7", "ee858d"], # [BANK] U+E667 -> U+E14D
  ["ee99a8", "ee8594"], # [AUTOMATED TELLER MACHINE] U+E668 -> U+E154
  ["ee99a9", "ee8598"], # [HOTEL] U+E669 -> U+E158
  ["ee99aa", "ee8596"], # [CONVENIENCE STORE] U+E66A -> U+E156
  ["ee9cbe", "ee8597"], # [SCHOOL] U+E73E -> U+E157
  ["ee9d80", "ee80bb"], # [MOUNT FUJI] U+E740 -> U+E03B
  ["ee9a99", "ee8087"], # [ATHLETIC SHOE] U+E699 -> U+E007
  ["ee99b4", "ee84be"], # [HIGH-HEELED SHOE] U+E674 -> U+E13E
  ["ee9a98", "ee94b6"], # [FOOTPRINTS] U+E698 -> U+E536
  ["ee9a9a", :undef], # [EYEGLASSES] U+E69A -> "[メガネ]"
  ["ee9c8e", "ee8086"], # [T-SHIRT] U+E70E -> U+E006
  ["ee9c91", :undef], # [JEANS] U+E711 -> "[ジーンズ]"
  ["ee9c9a", "ee848e"], # [CROWN] U+E71A -> U+E10E
  ["ee9c8f", :undef], # [PURSE] U+E70F -> "[財布]"
  ["ee9a82", "ee8ca3"], # [HANDBAG] U+E682 -> U+E323
  ["ee9aad", :undef], # [POUCH] U+E6AD -> "[ふくろ]"
  ["ee9c95", "ee84af"], # [MONEY BAG] U+E715 -> U+E12F
  ["ee9b96", :undef], # [BANKNOTE WITH YEN SIGN] U+E6D6 -> "￥"
  ["ee9c98", :undef], # [WRENCH] U+E718 -> "[レンチ]"
  ["ee9a84", "ee8c94"], # [RIBBON] U+E684 -> U+E314
  ["ee9a85", "ee8492"], # [WRAPPED PRESENT] U+E685 -> U+E112
  ["ee9a86", "ee8d8b"], # [BIRTHDAY CAKE] U+E686 -> U+E34B
  ["ee9aa4", "ee80b3"], # [CHRISTMAS TREE] U+E6A4 -> U+E033
  ["ee999a", :undef], # [PAGER] U+E65A -> "[ポケベル]"
  ["ee9a87", "ee8089"], # [BLACK TELEPHONE] U+E687 -> U+E009
  ["ee9a88", "ee808a"], # [MOBILE PHONE] U+E688 -> U+E00A
  ["ee9b8e", "ee8484"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+E6CE -> U+E104
  ["ee9a89", "ee8c81"], # [MEMO] U+E689 -> U+E301
  ["ee9b90", "ee808b"], # [FAX MACHINE] U+E6D0 -> U+E00B
  ["ee9b93", "ee8483"], # [ENVELOPE] U+E6D3 -> U+E103
  ["ee9b8f", "ee8483"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+E6CF -> U+E103
  ["ee9aae", :undef], # [BLACK NIB] U+E6AE -> "[ペン]"
  ["ee9ab2", "ee849f"], # [SEAT] U+E6B2 -> U+E11F
  ["ee9c96", "ee808c"], # [PERSONAL COMPUTER] U+E716 -> U+E00C
  ["ee9c99", "ee8c81"], # [PENCIL] U+E719 -> U+E301
  ["ee9cb0", :undef], # [PAPERCLIP] U+E730 -> "[クリップ]"
  ["ee9a8c", "ee84a6"], # [OPTICAL DISC] U+E68C -> U+E126
  ["ee99b5", "ee8c93"], # [BLACK SCISSORS] U+E675 -> U+E313
  ["ee9a83", "ee8588"], # [OPEN BOOK] U+E683 -> U+E148
  ["ee9992", :undef], # [RUNNING SHIRT WITH SASH] U+E652 -> U+3013 (GETA)
  ["ee9993", "ee8096"], # [BASEBALL] U+E653 -> U+E016
  ["ee9994", "ee8094"], # [FLAG IN HOLE] U+E654 -> U+E014
  ["ee9995", "ee8095"], # [TENNIS RACQUET AND BALL] U+E655 -> U+E015
  ["ee9996", "ee8098"], # [SOCCER BALL] U+E656 -> U+E018
  ["ee9997", "ee8093"], # [SKI AND SKI BOOT] U+E657 -> U+E013
  ["ee9998", "ee90aa"], # [BASKETBALL AND HOOP] U+E658 -> U+E42A
  ["ee9999", "ee84b2"], # [CHEQUERED FLAG] U+E659 -> U+E132
  ["ee9c92", :undef], # [SNOWBOARDER] U+E712 -> "[スノボ]"
  ["ee9cb3", "ee8495"], # [RUNNER] U+E733 -> U+E115
  ["ee999b", "ee809e"], # [TRAIN] U+E65B -> U+E01E
  ["ee999c", "ee90b4"], # [CIRCLED LATIN CAPITAL LETTER M] U+E65C -> U+E434
  ["ee999d", "ee90b5"], # [HIGH-SPEED TRAIN] U+E65D -> U+E435
  ["ee999e", "ee809b"], # [AUTOMOBILE] U+E65E -> U+E01B
  ["ee999f", "ee90ae"], # [RECREATIONAL VEHICLE] U+E65F -> U+E42E
  ["ee99a0", "ee8599"], # [ONCOMING BUS] U+E660 -> U+E159
  ["ee99a1", "ee8882"], # [SHIP] U+E661 -> U+E202
  ["ee99a2", "ee809d"], # [AIRPLANE] U+E662 -> U+E01D
  ["ee9aa3", "ee809c"], # [SAILBOAT] U+E6A3 -> U+E01C
  ["ee99ab", "ee80ba"], # [FUEL PUMP] U+E66B -> U+E03A
  ["ee99ac", "ee858f"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E66C -> U+E14F
  ["ee99ad", "ee858e"], # [HORIZONTAL TRAFFIC LIGHT] U+E66D -> U+E14E
  ["ee9bb7", "ee84a3"], # [HOT SPRINGS] U+E6F7 -> U+E123
  ["ee99b9", :undef], # [CAROUSEL HORSE] U+E679 -> U+3013 (GETA)
  ["ee99b6", "ee80bc"], # [MICROPHONE] U+E676 -> U+E03C
  ["ee99b7", "ee80bd"], # [MOVIE CAMERA] U+E677 -> U+E03D
  ["ee99ba", "ee8c8a"], # [HEADPHONE] U+E67A -> U+E30A
  ["ee99bb", "ee9482"], # [ARTIST PALETTE] U+E67B -> U+E502
  ["ee99bc", "ee9483"], # [TOP HAT] U+E67C -> U+E503
  ["ee99bd", :undef], # [CIRCUS TENT] U+E67D -> "[イベント]"
  ["ee99be", "ee84a5"], # [TICKET] U+E67E -> U+E125
  ["ee9aac", "ee8ca4"], # [CLAPPER BOARD] U+E6AC -> U+E324
  ["ee9a8b", :undef], # [VIDEO GAME] U+E68B -> "[ゲーム]"
  ["ee9bb6", "ee80be"], # [MUSICAL NOTE] U+E6F6 -> U+E03E
  ["ee9bbf", "ee8ca6"], # [MULTIPLE MUSICAL NOTES] U+E6FF -> U+E326
  ["ee9a81", "ee8088"], # [CAMERA] U+E681 -> U+E008
  ["ee9a8a", "ee84aa"], # [TELEVISION] U+E68A -> U+E12A
  ["ee9bb9", "ee8083"], # [KISS MARK] U+E6F9 -> U+E003
  ["ee9c97", "ee8483ee8ca8"], # [LOVE LETTER] U+E717 -> U+E103 U+E328
  ["ee9c9b", "ee80b4"], # [RING] U+E71B -> U+E034
  ["ee9cb1", "ee898e"], # [COPYRIGHT SIGN] U+E731 -> U+E24E
  ["ee9cb6", "ee898f"], # [REGISTERED SIGN] U+E736 -> U+E24F
  ["ee9cb2", "ee94b7"], # [TRADE MARK SIGN] U+E732 -> U+E537
  ["ee9ba0", "ee8890"], # [HASH KEY] U+E6E0 -> U+E210
  ["ee9ba2", "ee889c"], # [KEYCAP 1] U+E6E2 -> U+E21C
  ["ee9ba3", "ee889d"], # [KEYCAP 2] U+E6E3 -> U+E21D
  ["ee9ba4", "ee889e"], # [KEYCAP 3] U+E6E4 -> U+E21E
  ["ee9ba5", "ee889f"], # [KEYCAP 4] U+E6E5 -> U+E21F
  ["ee9ba6", "ee88a0"], # [KEYCAP 5] U+E6E6 -> U+E220
  ["ee9ba7", "ee88a1"], # [KEYCAP 6] U+E6E7 -> U+E221
  ["ee9ba8", "ee88a2"], # [KEYCAP 7] U+E6E8 -> U+E222
  ["ee9ba9", "ee88a3"], # [KEYCAP 8] U+E6E9 -> U+E223
  ["ee9baa", "ee88a4"], # [KEYCAP 9] U+E6EA -> U+E224
  ["ee9bab", "ee88a5"], # [KEYCAP 0] U+E6EB -> U+E225
  ["ee99b3", "ee84a0"], # [HAMBURGER] U+E673 -> U+E120
  ["ee9d89", "ee8d82"], # [RICE BALL] U+E749 -> U+E342
  ["ee9d8a", "ee8186"], # [SHORTCAKE] U+E74A -> U+E046
  ["ee9d8c", "ee8d80"], # [STEAMING BOWL] U+E74C -> U+E340
  ["ee9d8d", "ee8cb9"], # [BREAD] U+E74D -> U+E339
  ["ee99af", "ee8183"], # [FORK AND KNIFE] U+E66F -> U+E043
  ["ee99b0", "ee8185"], # [HOT BEVERAGE] U+E670 -> U+E045
  ["ee99b1", "ee8184"], # [COCKTAIL GLASS] U+E671 -> U+E044
  ["ee99b2", "ee8187"], # [BEER MUG] U+E672 -> U+E047
  ["ee9c9e", "ee8cb8"], # [TEACUP WITHOUT HANDLE] U+E71E -> U+E338
  ["ee9d8b", "ee8c8b"], # [SAKE BOTTLE AND CUP] U+E74B -> U+E30B
  ["ee9d96", "ee8184"], # [WINE GLASS] U+E756 -> U+E044
  ["ee99b8", "ee88b6"], # [NORTH EAST ARROW] U+E678 -> U+E236
  ["ee9a96", "ee88b8"], # [SOUTH EAST ARROW] U+E696 -> U+E238
  ["ee9a97", "ee88b7"], # [NORTH WEST ARROW] U+E697 -> U+E237
  ["ee9aa5", "ee88b9"], # [SOUTH WEST ARROW] U+E6A5 -> U+E239
  ["ee9bb5", "ee88b6"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+E6F5 -> U+E236
  ["ee9c80", "ee88b8"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+E700 -> U+E238
  ["ee9cbc", :undef], # [LEFT RIGHT ARROW] U+E73C -> "⇔"
  ["ee9cbd", :undef], # [UP DOWN ARROW] U+E73D -> "↑↓"
  ["ee9c82", "ee80a1"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E702 -> U+E021
  ["ee9c83", :undef], # [EXCLAMATION QUESTION MARK] U+E703 -> "！？"
  ["ee9c84", :undef], # [DOUBLE EXCLAMATION MARK] U+E704 -> "！！"
  ["ee9c89", :undef], # [WAVY DASH] U+E709 -> U+3013 (GETA)
  ["ee9c8a", :undef], # [CURLY LOOP] U+E70A -> "～"
  ["ee9b9f", "ee8891"], # [DOUBLE CURLY LOOP] U+E6DF -> U+E211
  ["ee9bac", "ee80a2"], # [HEAVY BLACK HEART] U+E6EC -> U+E022
  ["ee9bad", "ee8ca7"], # [BEATING HEART] U+E6ED -> U+E327
  ["ee9bae", "ee80a3"], # [BROKEN HEART] U+E6EE -> U+E023
  ["ee9baf", "ee8ca7"], # [TWO HEARTS] U+E6EF -> U+E327
  ["ee9a8d", "ee888c"], # [BLACK HEART SUIT] U+E68D -> U+E20C
  ["ee9a8e", "ee888e"], # [BLACK SPADE SUIT] U+E68E -> U+E20E
  ["ee9a8f", "ee888d"], # [BLACK DIAMOND SUIT] U+E68F -> U+E20D
  ["ee9a90", "ee888f"], # [BLACK CLUB SUIT] U+E690 -> U+E20F
  ["ee99bf", "ee8c8e"], # [SMOKING SYMBOL] U+E67F -> U+E30E
  ["ee9a80", "ee8888"], # [NO SMOKING SYMBOL] U+E680 -> U+E208
  ["ee9a9b", "ee888a"], # [WHEELCHAIR SYMBOL] U+E69B -> U+E20A
  ["ee9b9e", :undef], # [TRIANGULAR FLAG ON POST] U+E6DE -> "[旗]"
  ["ee9cb7", "ee8992"], # [WARNING SIGN] U+E737 -> U+E252
  ["ee9cb5", :undef], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+E735 -> "↑↓"
  ["ee9c9d", "ee84b6"], # [BICYCLE] U+E71D -> U+E136
  ["ee99ae", "ee8591"], # [RESTROOM] U+E66E -> U+E151
  ["ee9c94", :undef], # [DOOR] U+E714 -> "[ドア]"
  ["ee9b9b", :undef], # [SQUARED CL] U+E6DB -> "[CL]"
  ["ee9b97", :undef], # [SQUARED FREE] U+E6D7 -> "[FREE]"
  ["ee9b98", "ee88a9"], # [SQUARED ID] U+E6D8 -> U+E229
  ["ee9b9d", "ee8892"], # [SQUARED NEW] U+E6DD -> U+E212
  ["ee9caf", :undef], # [SQUARED NG] U+E72F -> "[NG]"
  ["ee9c8b", "ee898d"], # [SQUARED OK] U+E70B -> U+E24D
  ["ee9cb8", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7981] U+E738 -> "[禁]"
  ["ee9cb9", "ee88ab"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+E739 -> U+E22B
  ["ee9cba", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5408] U+E73A -> "[合]"
  ["ee9cbb", "ee88aa"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+E73B -> U+E22A
  ["ee9cb4", "ee8c95"], # [CIRCLED IDEOGRAPH SECRET] U+E734 -> U+E315
  ["ee9bb8", :undef], # [DIAMOND SHAPE WITH A DOT INSIDE] U+E6F8 -> U+3013 (GETA)
  ["ee9bbb", "ee848f"], # [ELECTRIC LIGHT BULB] U+E6FB -> U+E10F
  ["ee9bbc", "ee8cb4"], # [ANGER SYMBOL] U+E6FC -> U+E334
  ["ee9bbe", "ee8c91"], # [BOMB] U+E6FE -> U+E311
  ["ee9c81", "ee84bc"], # [SLEEPING SYMBOL] U+E701 -> U+E13C
  ["ee9c85", :undef], # [COLLISION SYMBOL] U+E705 -> "[ドンッ]"
  ["ee9c86", "ee8cb1"], # [SPLASHING SWEAT SYMBOL] U+E706 -> U+E331
  ["ee9c87", "ee8cb1"], # [DROP OF WATER] U+E707 -> U+E331
  ["ee9c88", "ee8cb0"], # [DASH SYMBOL] U+E708 -> U+E330
  ["ee9bba", "ee8cae"], # [SPARKLES] U+E6FA -> U+E32E
  ["ee9b9a", :undef], # [LEFTWARDS ARROW WITH HOOK] U+E6DA -> "←┘"
  ["ee9b9c", "ee8494"], # [LEFT-POINTING MAGNIFYING GLASS] U+E6DC -> U+E114
  ["ee9b99", "ee80bf"], # [KEY] U+E6D9 -> U+E03F
  ["ee9c93", "ee8ca5"], # [BELL] U+E713 -> U+E325
  ["ee9ab9", :undef], # [END WITH LEFTWARDS ARROW ABOVE] U+E6B9 -> "[end]"
  ["ee9ab8", :undef], # [ON WITH EXCLAMATION MARK WITH LEFT RIGHT ARROW ABOVE] U+E6B8 -> "[ON]"
  ["ee9ab7", :undef], # [SOON WITH RIGHTWARDS ARROW ABOVE] U+E6B7 -> "[SOON]"
  ["ee9a93", "ee8090"], # [RAISED FIST] U+E693 -> U+E010
  ["ee9a95", "ee8092"], # [RAISED HAND] U+E695 -> U+E012
  ["ee9a94", "ee8091"], # [VICTORY HAND] U+E694 -> U+E011
  ["ee9bbd", "ee808d"], # [FISTED HAND SIGN] U+E6FD -> U+E00D
  ["ee9ca7", "ee808e"], # [THUMBS UP SIGN] U+E727 -> U+E00E
  ["ee9b91", :undef], # [EMOJI COMPATIBILITY SYMBOL-1] U+E6D1 -> "[iモード]"
  ["ee9b92", :undef], # [EMOJI COMPATIBILITY SYMBOL-2] U+E6D2 -> "[iモード]"
  ["ee9b94", :undef], # [EMOJI COMPATIBILITY SYMBOL-3] U+E6D4 -> "[ドコモ]"
  ["ee9b95", :undef], # [EMOJI COMPATIBILITY SYMBOL-4] U+E6D5 -> "[ドコモポイント]"
  ["ee9c8c", :undef], # [EMOJI COMPATIBILITY SYMBOL-5] U+E70C -> "[iアプリ]"
  ["ee9c8d", :undef], # [EMOJI COMPATIBILITY SYMBOL-6] U+E70D -> "[iアプリ]"
  ["ee9aa6", :undef], # [EMOJI COMPATIBILITY SYMBOL-7] U+E6A6 -> "[ぴ]"
  ["ee9aa7", :undef], # [EMOJI COMPATIBILITY SYMBOL-8] U+E6A7 -> "[あ]"
  ["ee9aa8", :undef], # [EMOJI COMPATIBILITY SYMBOL-9] U+E6A8 -> "[チケット]"
  ["ee9aa9", :undef], # [EMOJI COMPATIBILITY SYMBOL-10] U+E6A9 -> "[チケット]"
  ["ee9aaa", :undef], # [EMOJI COMPATIBILITY SYMBOL-11] U+E6AA -> "[電話先行]"
  ["ee9aab", :undef], # [EMOJI COMPATIBILITY SYMBOL-12] U+E6AB -> "[Pコード]"
  ["ee9aaf", "ee80bd"], # [EMOJI COMPATIBILITY SYMBOL-13] U+E6AF -> U+E03D
  ["ee9ab0", :undef], # [EMOJI COMPATIBILITY SYMBOL-14] U+E6B0 -> "[ぴ]"
  ["ee9ab4", :undef], # [EMOJI COMPATIBILITY SYMBOL-15] U+E6B4 -> "(ぴ)"
  ["ee9ab5", :undef], # [EMOJI COMPATIBILITY SYMBOL-16] U+E6B5 -> "[ぴ]"
  ["ee9ab6", :undef], # [EMOJI COMPATIBILITY SYMBOL-17] U+E6B6 -> "[チェック]"
  ["ee9abb", :undef], # [EMOJI COMPATIBILITY SYMBOL-18] U+E6BB -> "[F]"
  ["ee9abc", :undef], # [EMOJI COMPATIBILITY SYMBOL-19] U+E6BC -> "[D]"
  ["ee9abd", :undef], # [EMOJI COMPATIBILITY SYMBOL-20] U+E6BD -> "[S]"
  ["ee9abe", :undef], # [EMOJI COMPATIBILITY SYMBOL-21] U+E6BE -> "[C]"
  ["ee9abf", :undef], # [EMOJI COMPATIBILITY SYMBOL-22] U+E6BF -> "[R]"
  ["ee9b80", :undef], # [EMOJI COMPATIBILITY SYMBOL-23] U+E6C0 -> "■"
  ["ee9b81", :undef], # [EMOJI COMPATIBILITY SYMBOL-24] U+E6C1 -> "■"
  ["ee9b82", :undef], # [EMOJI COMPATIBILITY SYMBOL-25] U+E6C2 -> "▼"
  ["ee9b83", :undef], # [EMOJI COMPATIBILITY SYMBOL-26] U+E6C3 -> "††††"
  ["ee9b84", :undef], # [EMOJI COMPATIBILITY SYMBOL-27] U+E6C4 -> "†††"
  ["ee9b85", :undef], # [EMOJI COMPATIBILITY SYMBOL-28] U+E6C5 -> "††"
  ["ee9b86", :undef], # [EMOJI COMPATIBILITY SYMBOL-29] U+E6C6 -> "†"
  ["ee9b87", :undef], # [EMOJI COMPATIBILITY SYMBOL-30] U+E6C7 -> "[I]"
  ["ee9b88", :undef], # [EMOJI COMPATIBILITY SYMBOL-31] U+E6C8 -> "[M]"
  ["ee9b89", :undef], # [EMOJI COMPATIBILITY SYMBOL-32] U+E6C9 -> "[E]"
  ["ee9b8a", :undef], # [EMOJI COMPATIBILITY SYMBOL-33] U+E6CA -> "[VE]"
  ["ee9b8b", :undef], # [EMOJI COMPATIBILITY SYMBOL-34] U+E6CB -> "●"
  ["ee9b8c", :undef], # [EMOJI COMPATIBILITY SYMBOL-35] U+E6CC -> "[カード使用不可]"
  ["ee9b8d", :undef], # [EMOJI COMPATIBILITY SYMBOL-36] U+E6CD -> "[チェックボックス]"
  ["ee9ba1", :undef], # [EMOJI COMPATIBILITY SYMBOL-66] U+E6E1 -> "[Q]"
]

EMOJI_EXCHANGE_TBL['UTF8-DoCoMo']['UTF-8'] = [
  ["ee98be", "e29880"], # [BLACK SUN WITH RAYS] U+E63E -> U+2600
  ["ee98bf", "e29881"], # [CLOUD] U+E63F -> U+2601
  ["ee9980", "e29894"], # [UMBRELLA WITH RAIN DROPS] U+E640 -> U+2614
  ["ee9981", "e29b84"], # [SNOWMAN WITHOUT SNOW] U+E641 -> U+26C4
  ["ee9982", "e29aa1"], # [HIGH VOLTAGE SIGN] U+E642 -> U+26A1
  ["ee9983", :undef], # [CYCLONE] U+E643 -> U+1F300
  ["ee9984", :undef], # [FOGGY] U+E644 -> U+1F301
  ["ee9985", :undef], # [CLOSED UMBRELLA] U+E645 -> U+1F302
  ["ee9ab3", :undef], # [NIGHT WITH STARS] U+E6B3 -> U+1F303
  ["ee9cbf", :undef], # [WATER WAVE] U+E73F -> U+1F30A
  ["ee9a9c", :undef], # [NEW MOON SYMBOL] U+E69C -> U+1F311
  ["ee9a9d", :undef], # [WAXING GIBBOUS MOON SYMBOL] U+E69D -> U+1F314
  ["ee9a9e", :undef], # [FIRST QUARTER MOON SYMBOL] U+E69E -> U+1F313
  ["ee9a9f", :undef], # [CRESCENT MOON] U+E69F -> U+1F319
  ["ee9aa0", :undef], # [FULL MOON SYMBOL] U+E6A0 -> U+1F315
  ["ee9c9f", "e28c9a"], # [WATCH] U+E71F -> U+231A
  ["ee9aba", :undef], # [ALARM CLOCK] U+E6BA -> U+23F0
  ["ee9c9c", :undef], # [HOURGLASS WITH FLOWING SAND] U+E71C -> U+23F3
  ["ee9986", "e29988"], # [ARIES] U+E646 -> U+2648
  ["ee9987", "e29989"], # [TAURUS] U+E647 -> U+2649
  ["ee9988", "e2998a"], # [GEMINI] U+E648 -> U+264A
  ["ee9989", "e2998b"], # [CANCER] U+E649 -> U+264B
  ["ee998a", "e2998c"], # [LEO] U+E64A -> U+264C
  ["ee998b", "e2998d"], # [VIRGO] U+E64B -> U+264D
  ["ee998c", "e2998e"], # [LIBRA] U+E64C -> U+264E
  ["ee998d", "e2998f"], # [SCORPIUS] U+E64D -> U+264F
  ["ee998e", "e29990"], # [SAGITTARIUS] U+E64E -> U+2650
  ["ee998f", "e29991"], # [CAPRICORN] U+E64F -> U+2651
  ["ee9990", "e29992"], # [AQUARIUS] U+E650 -> U+2652
  ["ee9991", "e29993"], # [PISCES] U+E651 -> U+2653
  ["ee9d81", :undef], # [FOUR LEAF CLOVER] U+E741 -> U+1F340
  ["ee9d83", :undef], # [TULIP] U+E743 -> U+1F337
  ["ee9d86", :undef], # [SEEDLING] U+E746 -> U+1F331
  ["ee9d87", :undef], # [MAPLE LEAF] U+E747 -> U+1F341
  ["ee9d88", :undef], # [CHERRY BLOSSOM] U+E748 -> U+1F338
  ["ee9d82", :undef], # [CHERRIES] U+E742 -> U+1F352
  ["ee9d84", :undef], # [BANANA] U+E744 -> U+1F34C
  ["ee9d85", :undef], # [RED APPLE] U+E745 -> U+1F34E
  ["ee9a91", :undef], # [EYES] U+E691 -> U+1F440
  ["ee9a92", :undef], # [EAR] U+E692 -> U+1F442
  ["ee9c90", :undef], # [LIPSTICK] U+E710 -> U+1F484
  ["ee9ab1", :undef], # [BUST IN SILHOUETTE] U+E6B1 -> U+1F464
  ["ee9d8e", :undef], # [SNAIL] U+E74E -> U+1F40C
  ["ee9d8f", :undef], # [BABY CHICK] U+E74F -> U+1F424
  ["ee9d90", :undef], # [PENGUIN] U+E750 -> U+1F427
  ["ee9d91", :undef], # [FISH] U+E751 -> U+1F41F
  ["ee9aa2", :undef], # [CAT FACE] U+E6A2 -> U+1F431
  ["ee9d94", :undef], # [HORSE FACE] U+E754 -> U+1F434
  ["ee9aa1", :undef], # [DOG FACE] U+E6A1 -> U+1F436
  ["ee9d95", :undef], # [PIG FACE] U+E755 -> U+1F437
  ["ee9bb1", :undef], # [ANGRY FACE] U+E6F1 -> U+1F600
  ["ee9bb2", :undef], # [DISAPPOINTED FACE] U+E6F2 -> U+1F603
  ["ee9bb4", :undef], # [DIZZY FACE] U+E6F4 -> U+1F604
  ["ee9ca5", :undef], # [EXPRESSIONLESS FACE] U+E725 -> U+1F606
  ["ee9ca6", :undef], # [FACE WITH HEART-SHAPED EYES] U+E726 -> U+1F607
  ["ee9ca8", :undef], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E728 -> U+1F609
  ["ee9d92", :undef], # [FACE SAVOURING DELICIOUS FOOD] U+E752 -> U+1F60B
  ["ee9bb0", :undef], # [HAPPY FACE WITH OPEN MOUTH] U+E6F0 -> U+1F610
  ["ee9ca2", :undef], # [HAPPY FACE WITH OPEN MOUTH AND COLD SWEAT] U+E722 -> U+1F611
  ["ee9caa", :undef], # [HAPPY FACE WITH OPEN MOUTH AND CLOSED EYES] U+E72A -> U+1F612
  ["ee9d93", :undef], # [HAPPY FACE WITH GRIN] U+E753 -> U+1F613
  ["ee9cae", :undef], # [CRYING FACE] U+E72E -> U+1F617
  ["ee9cad", :undef], # [LOUDLY CRYING FACE] U+E72D -> U+1F618
  ["ee9cab", :undef], # [PERSEVERING FACE] U+E72B -> U+1F61A
  ["ee9ca4", :undef], # [POUTING FACE] U+E724 -> U+1F61B
  ["ee9ca1", :undef], # [RELIEVED FACE] U+E721 -> U+1F61C
  ["ee9bb3", :undef], # [CONFOUNDED FACE] U+E6F3 -> U+1F61D
  ["ee9ca0", :undef], # [PENSIVE FACE] U+E720 -> U+1F61E
  ["ee9d97", :undef], # [FACE SCREAMING IN FEAR] U+E757 -> U+1F61F
  ["ee9cac", :undef], # [SMIRKING FACE] U+E72C -> U+1F621
  ["ee9ca3", :undef], # [FACE WITH COLD SWEAT] U+E723 -> U+1F622
  ["ee9ca9", :undef], # [WINKING FACE] U+E729 -> U+1F625
  ["ee99a3", :undef], # [HOUSE BUILDING] U+E663 -> U+1F3E0
  ["ee99a4", :undef], # [OFFICE BUILDING] U+E664 -> U+1F3E2
  ["ee99a5", :undef], # [JAPANESE POST OFFICE] U+E665 -> U+1F3E3
  ["ee99a6", :undef], # [HOSPITAL] U+E666 -> U+1F3E5
  ["ee99a7", :undef], # [BANK] U+E667 -> U+1F3E6
  ["ee99a8", :undef], # [AUTOMATED TELLER MACHINE] U+E668 -> U+1F3E7
  ["ee99a9", :undef], # [HOTEL] U+E669 -> U+1F3E8
  ["ee99aa", :undef], # [CONVENIENCE STORE] U+E66A -> U+1F3EA
  ["ee9cbe", :undef], # [SCHOOL] U+E73E -> U+1F3EB
  ["ee9d80", :undef], # [MOUNT FUJI] U+E740 -> U+1F5FB
  ["ee9a99", :undef], # [ATHLETIC SHOE] U+E699 -> U+1F45F
  ["ee99b4", :undef], # [HIGH-HEELED SHOE] U+E674 -> U+1F460
  ["ee9a98", :undef], # [FOOTPRINTS] U+E698 -> U+1F463
  ["ee9a9a", :undef], # [EYEGLASSES] U+E69A -> U+1F453
  ["ee9c8e", :undef], # [T-SHIRT] U+E70E -> U+1F455
  ["ee9c91", :undef], # [JEANS] U+E711 -> U+1F456
  ["ee9c9a", :undef], # [CROWN] U+E71A -> U+1F451
  ["ee9c8f", :undef], # [PURSE] U+E70F -> U+1F45B
  ["ee9a82", :undef], # [HANDBAG] U+E682 -> U+1F45C
  ["ee9aad", :undef], # [POUCH] U+E6AD -> U+1F45D
  ["ee9c95", :undef], # [MONEY BAG] U+E715 -> U+1F4B0
  ["ee9b96", :undef], # [BANKNOTE WITH YEN SIGN] U+E6D6 -> U+1F4B4
  ["ee9c98", :undef], # [WRENCH] U+E718 -> U+1F527
  ["ee9a84", :undef], # [RIBBON] U+E684 -> U+1F380
  ["ee9a85", :undef], # [WRAPPED PRESENT] U+E685 -> U+1F381
  ["ee9a86", :undef], # [BIRTHDAY CAKE] U+E686 -> U+1F382
  ["ee9aa4", :undef], # [CHRISTMAS TREE] U+E6A4 -> U+1F384
  ["ee999a", :undef], # [PAGER] U+E65A -> U+1F4DF
  ["ee9a87", "e2988e"], # [BLACK TELEPHONE] U+E687 -> U+260E
  ["ee9a88", :undef], # [MOBILE PHONE] U+E688 -> U+1F4F1
  ["ee9b8e", :undef], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+E6CE -> U+1F4F2
  ["ee9a89", :undef], # [MEMO] U+E689 -> U+1F4DD
  ["ee9b90", :undef], # [FAX MACHINE] U+E6D0 -> U+1F4E0
  ["ee9b93", "e29c89"], # [ENVELOPE] U+E6D3 -> U+2709
  ["ee9b8f", :undef], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+E6CF -> U+1F4E9
  ["ee9aae", "e29c92"], # [BLACK NIB] U+E6AE -> U+2712
  ["ee9ab2", :undef], # [SEAT] U+E6B2 -> U+1F4BA
  ["ee9c96", :undef], # [PERSONAL COMPUTER] U+E716 -> U+1F4BB
  ["ee9c99", "e29c8f"], # [PENCIL] U+E719 -> U+270F
  ["ee9cb0", :undef], # [PAPERCLIP] U+E730 -> U+1F4CE
  ["ee9a8c", :undef], # [OPTICAL DISC] U+E68C -> U+1F4BF
  ["ee99b5", "e29c82"], # [BLACK SCISSORS] U+E675 -> U+2702
  ["ee9a83", :undef], # [OPEN BOOK] U+E683 -> U+1F4D6
  ["ee9992", :undef], # [RUNNING SHIRT WITH SASH] U+E652 -> U+1F3BD
  ["ee9993", "e29abe"], # [BASEBALL] U+E653 -> U+26BE
  ["ee9994", "e29bb3"], # [FLAG IN HOLE] U+E654 -> U+26F3
  ["ee9995", :undef], # [TENNIS RACQUET AND BALL] U+E655 -> U+1F3BE
  ["ee9996", "e29abd"], # [SOCCER BALL] U+E656 -> U+26BD
  ["ee9997", :undef], # [SKI AND SKI BOOT] U+E657 -> U+1F3BF
  ["ee9998", :undef], # [BASKETBALL AND HOOP] U+E658 -> U+1F3C0
  ["ee9999", :undef], # [CHEQUERED FLAG] U+E659 -> U+1F3C1
  ["ee9c92", :undef], # [SNOWBOARDER] U+E712 -> U+1F3C2
  ["ee9cb3", :undef], # [RUNNER] U+E733 -> U+1F3C3
  ["ee999b", :undef], # [TRAIN] U+E65B -> U+1F686
  ["ee999c", "e29382"], # [CIRCLED LATIN CAPITAL LETTER M] U+E65C -> U+24C2
  ["ee999d", :undef], # [HIGH-SPEED TRAIN] U+E65D -> U+1F684
  ["ee999e", :undef], # [AUTOMOBILE] U+E65E -> U+1F697
  ["ee999f", :undef], # [RECREATIONAL VEHICLE] U+E65F -> U+1F699
  ["ee99a0", :undef], # [ONCOMING BUS] U+E660 -> U+1F68D
  ["ee99a1", :undef], # [SHIP] U+E661 -> U+1F6A2
  ["ee99a2", "e29c88"], # [AIRPLANE] U+E662 -> U+2708
  ["ee9aa3", "e29bb5"], # [SAILBOAT] U+E6A3 -> U+26F5
  ["ee99ab", "e29bbd"], # [FUEL PUMP] U+E66B -> U+26FD
  ["ee99ac", "f09f85bf"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E66C -> U+1F17F
  ["ee99ad", :undef], # [HORIZONTAL TRAFFIC LIGHT] U+E66D -> U+1F6A5
  ["ee9bb7", "e299a8"], # [HOT SPRINGS] U+E6F7 -> U+2668
  ["ee99b9", :undef], # [CAROUSEL HORSE] U+E679 -> U+1F3A0
  ["ee99b6", :undef], # [MICROPHONE] U+E676 -> U+1F3A4
  ["ee99b7", :undef], # [MOVIE CAMERA] U+E677 -> U+1F3A5
  ["ee99ba", :undef], # [HEADPHONE] U+E67A -> U+1F3A7
  ["ee99bb", :undef], # [ARTIST PALETTE] U+E67B -> U+1F3A8
  ["ee99bc", :undef], # [TOP HAT] U+E67C -> U+1F3A9
  ["ee99bd", :undef], # [CIRCUS TENT] U+E67D -> U+1F3AA
  ["ee99be", :undef], # [TICKET] U+E67E -> U+1F3AB
  ["ee9aac", :undef], # [CLAPPER BOARD] U+E6AC -> U+1F3AC
  ["ee9a8b", :undef], # [VIDEO GAME] U+E68B -> U+1F3AE
  ["ee9bb6", :undef], # [MUSICAL NOTE] U+E6F6 -> U+1F3B5
  ["ee9bbf", :undef], # [MULTIPLE MUSICAL NOTES] U+E6FF -> U+1F3B6
  ["ee9a81", :undef], # [CAMERA] U+E681 -> U+1F4F7
  ["ee9a8a", :undef], # [TELEVISION] U+E68A -> U+1F4FA
  ["ee9bb9", :undef], # [KISS MARK] U+E6F9 -> U+1F48B
  ["ee9c97", :undef], # [LOVE LETTER] U+E717 -> U+1F48C
  ["ee9c9b", :undef], # [RING] U+E71B -> U+1F48D
  ["ee9cb1", "c2a9"], # [COPYRIGHT SIGN] U+E731 -> U+A9
  ["ee9cb6", "c2ae"], # [REGISTERED SIGN] U+E736 -> U+AE
  ["ee9cb2", "e284a2"], # [TRADE MARK SIGN] U+E732 -> U+2122
  ["ee9ba0", "23e283a3"], # [HASH KEY] U+E6E0 -> U+23 U+20E3
  ["ee9ba2", "31e283a3"], # [KEYCAP 1] U+E6E2 -> U+31 U+20E3
  ["ee9ba3", "32e283a3"], # [KEYCAP 2] U+E6E3 -> U+32 U+20E3
  ["ee9ba4", "33e283a3"], # [KEYCAP 3] U+E6E4 -> U+33 U+20E3
  ["ee9ba5", "34e283a3"], # [KEYCAP 4] U+E6E5 -> U+34 U+20E3
  ["ee9ba6", "35e283a3"], # [KEYCAP 5] U+E6E6 -> U+35 U+20E3
  ["ee9ba7", "36e283a3"], # [KEYCAP 6] U+E6E7 -> U+36 U+20E3
  ["ee9ba8", "37e283a3"], # [KEYCAP 7] U+E6E8 -> U+37 U+20E3
  ["ee9ba9", "38e283a3"], # [KEYCAP 8] U+E6E9 -> U+38 U+20E3
  ["ee9baa", "39e283a3"], # [KEYCAP 9] U+E6EA -> U+39 U+20E3
  ["ee9bab", "30e283a3"], # [KEYCAP 0] U+E6EB -> U+30 U+20E3
  ["ee99b3", :undef], # [HAMBURGER] U+E673 -> U+1F354
  ["ee9d89", :undef], # [RICE BALL] U+E749 -> U+1F359
  ["ee9d8a", :undef], # [SHORTCAKE] U+E74A -> U+1F370
  ["ee9d8c", :undef], # [STEAMING BOWL] U+E74C -> U+1F35C
  ["ee9d8d", :undef], # [BREAD] U+E74D -> U+1F35E
  ["ee99af", :undef], # [FORK AND KNIFE] U+E66F -> U+1F374
  ["ee99b0", "e29895"], # [HOT BEVERAGE] U+E670 -> U+2615
  ["ee99b1", :undef], # [COCKTAIL GLASS] U+E671 -> U+1F378
  ["ee99b2", :undef], # [BEER MUG] U+E672 -> U+1F37A
  ["ee9c9e", :undef], # [TEACUP WITHOUT HANDLE] U+E71E -> U+1F375
  ["ee9d8b", :undef], # [SAKE BOTTLE AND CUP] U+E74B -> U+1F376
  ["ee9d96", :undef], # [WINE GLASS] U+E756 -> U+1F377
  ["ee99b8", "e28697"], # [NORTH EAST ARROW] U+E678 -> U+2197
  ["ee9a96", "e28698"], # [SOUTH EAST ARROW] U+E696 -> U+2198
  ["ee9a97", "e28696"], # [NORTH WEST ARROW] U+E697 -> U+2196
  ["ee9aa5", "e28699"], # [SOUTH WEST ARROW] U+E6A5 -> U+2199
  ["ee9bb5", "e2a4b4"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+E6F5 -> U+2934
  ["ee9c80", "e2a4b5"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+E700 -> U+2935
  ["ee9cbc", "e28694"], # [LEFT RIGHT ARROW] U+E73C -> U+2194
  ["ee9cbd", "e28695"], # [UP DOWN ARROW] U+E73D -> U+2195
  ["ee9c82", "e29da2"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E702 -> U+2762
  ["ee9c83", "e28189"], # [EXCLAMATION QUESTION MARK] U+E703 -> U+2049
  ["ee9c84", "e280bc"], # [DOUBLE EXCLAMATION MARK] U+E704 -> U+203C
  ["ee9c89", "e380b0"], # [WAVY DASH] U+E709 -> U+3030
  ["ee9c8a", :undef], # [CURLY LOOP] U+E70A -> U+27B0
  ["ee9b9f", :undef], # [DOUBLE CURLY LOOP] U+E6DF -> U+27BF
  ["ee9bac", "e29da4"], # [HEAVY BLACK HEART] U+E6EC -> U+2764
  ["ee9bad", :undef], # [BEATING HEART] U+E6ED -> U+1F493
  ["ee9bae", :undef], # [BROKEN HEART] U+E6EE -> U+1F494
  ["ee9baf", :undef], # [TWO HEARTS] U+E6EF -> U+1F495
  ["ee9a8d", "e299a5"], # [BLACK HEART SUIT] U+E68D -> U+2665
  ["ee9a8e", "e299a0"], # [BLACK SPADE SUIT] U+E68E -> U+2660
  ["ee9a8f", "e299a6"], # [BLACK DIAMOND SUIT] U+E68F -> U+2666
  ["ee9a90", "e299a3"], # [BLACK CLUB SUIT] U+E690 -> U+2663
  ["ee99bf", :undef], # [SMOKING SYMBOL] U+E67F -> U+1F6AC
  ["ee9a80", :undef], # [NO SMOKING SYMBOL] U+E680 -> U+1F6AD
  ["ee9a9b", "e299bf"], # [WHEELCHAIR SYMBOL] U+E69B -> U+267F
  ["ee9b9e", :undef], # [TRIANGULAR FLAG ON POST] U+E6DE -> U+1F6A9
  ["ee9cb7", "e29aa0"], # [WARNING SIGN] U+E737 -> U+26A0
  ["ee9cb5", "e299bb"], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+E735 -> U+267B
  ["ee9c9d", :undef], # [BICYCLE] U+E71D -> U+1F6B2
  ["ee99ae", :undef], # [RESTROOM] U+E66E -> U+1F6BB
  ["ee9c94", :undef], # [DOOR] U+E714 -> U+1F6AA
  ["ee9b9b", :undef], # [SQUARED CL] U+E6DB -> U+1F191
  ["ee9b97", :undef], # [SQUARED FREE] U+E6D7 -> U+1F193
  ["ee9b98", :undef], # [SQUARED ID] U+E6D8 -> U+1F194
  ["ee9b9d", :undef], # [SQUARED NEW] U+E6DD -> U+1F195
  ["ee9caf", :undef], # [SQUARED NG] U+E72F -> U+1F196
  ["ee9c8b", :undef], # [SQUARED OK] U+E70B -> U+1F197
  ["ee9cb8", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7981] U+E738 -> U+1F232
  ["ee9cb9", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+E739 -> U+1F233
  ["ee9cba", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5408] U+E73A -> U+1F234
  ["ee9cbb", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+E73B -> U+1F235
  ["ee9cb4", "e38a99"], # [CIRCLED IDEOGRAPH SECRET] U+E734 -> U+3299
  ["ee9bb8", :undef], # [DIAMOND SHAPE WITH A DOT INSIDE] U+E6F8 -> U+1F4A0
  ["ee9bbb", :undef], # [ELECTRIC LIGHT BULB] U+E6FB -> U+1F4A1
  ["ee9bbc", :undef], # [ANGER SYMBOL] U+E6FC -> U+1F4A2
  ["ee9bbe", :undef], # [BOMB] U+E6FE -> U+1F4A3
  ["ee9c81", :undef], # [SLEEPING SYMBOL] U+E701 -> U+1F4A4
  ["ee9c85", :undef], # [COLLISION SYMBOL] U+E705 -> U+1F4A5
  ["ee9c86", :undef], # [SPLASHING SWEAT SYMBOL] U+E706 -> U+1F4A6
  ["ee9c87", :undef], # [DROP OF WATER] U+E707 -> U+1F4A7
  ["ee9c88", :undef], # [DASH SYMBOL] U+E708 -> U+1F4A8
  ["ee9bba", :undef], # [SPARKLES] U+E6FA -> U+2728
  ["ee9b9a", "e286a9"], # [LEFTWARDS ARROW WITH HOOK] U+E6DA -> U+21A9
  ["ee9b9c", :undef], # [LEFT-POINTING MAGNIFYING GLASS] U+E6DC -> U+1F50D
  ["ee9b99", :undef], # [KEY] U+E6D9 -> U+1F511
  ["ee9c93", :undef], # [BELL] U+E713 -> U+1F514
  ["ee9ab9", :undef], # [END WITH LEFTWARDS ARROW ABOVE] U+E6B9 -> U+1F51A
  ["ee9ab8", :undef], # [ON WITH EXCLAMATION MARK WITH LEFT RIGHT ARROW ABOVE] U+E6B8 -> U+1F51B
  ["ee9ab7", :undef], # [SOON WITH RIGHTWARDS ARROW ABOVE] U+E6B7 -> U+1F51C
  ["ee9a93", :undef], # [RAISED FIST] U+E693 -> U+270A
  ["ee9a95", :undef], # [RAISED HAND] U+E695 -> U+270B
  ["ee9a94", "e29c8c"], # [VICTORY HAND] U+E694 -> U+270C
  ["ee9bbd", :undef], # [FISTED HAND SIGN] U+E6FD -> U+1F44A
  ["ee9ca7", :undef], # [THUMBS UP SIGN] U+E727 -> U+1F44D
  ["ee9b91", :undef], # [EMOJI COMPATIBILITY SYMBOL-1] U+E6D1 -> "[iモード]"
  ["ee9b92", :undef], # [EMOJI COMPATIBILITY SYMBOL-2] U+E6D2 -> "[iモード]"
  ["ee9b94", :undef], # [EMOJI COMPATIBILITY SYMBOL-3] U+E6D4 -> "[ドコモ]"
  ["ee9b95", :undef], # [EMOJI COMPATIBILITY SYMBOL-4] U+E6D5 -> "[ドコモポイント]"
  ["ee9c8c", :undef], # [EMOJI COMPATIBILITY SYMBOL-5] U+E70C -> "[iアプリ]"
  ["ee9c8d", :undef], # [EMOJI COMPATIBILITY SYMBOL-6] U+E70D -> "[iアプリ]"
  ["ee9aa6", :undef], # [EMOJI COMPATIBILITY SYMBOL-7] U+E6A6 -> "[ぴ]"
  ["ee9aa7", :undef], # [EMOJI COMPATIBILITY SYMBOL-8] U+E6A7 -> "[あ]"
  ["ee9aa8", :undef], # [EMOJI COMPATIBILITY SYMBOL-9] U+E6A8 -> "[チケット]"
  ["ee9aa9", :undef], # [EMOJI COMPATIBILITY SYMBOL-10] U+E6A9 -> "[チケット]"
  ["ee9aaa", :undef], # [EMOJI COMPATIBILITY SYMBOL-11] U+E6AA -> "[電話先行]"
  ["ee9aab", :undef], # [EMOJI COMPATIBILITY SYMBOL-12] U+E6AB -> "[Pコード]"
  ["ee9aaf", :undef], # [EMOJI COMPATIBILITY SYMBOL-13] U+E6AF -> U+3013 (GETA)
  ["ee9ab0", :undef], # [EMOJI COMPATIBILITY SYMBOL-14] U+E6B0 -> "[ぴ]"
  ["ee9ab4", :undef], # [EMOJI COMPATIBILITY SYMBOL-15] U+E6B4 -> "(ぴ)"
  ["ee9ab5", :undef], # [EMOJI COMPATIBILITY SYMBOL-16] U+E6B5 -> "[ぴ]"
  ["ee9ab6", :undef], # [EMOJI COMPATIBILITY SYMBOL-17] U+E6B6 -> "[チェック]"
  ["ee9abb", :undef], # [EMOJI COMPATIBILITY SYMBOL-18] U+E6BB -> "[F]"
  ["ee9abc", :undef], # [EMOJI COMPATIBILITY SYMBOL-19] U+E6BC -> "[D]"
  ["ee9abd", :undef], # [EMOJI COMPATIBILITY SYMBOL-20] U+E6BD -> "[S]"
  ["ee9abe", :undef], # [EMOJI COMPATIBILITY SYMBOL-21] U+E6BE -> "[C]"
  ["ee9abf", :undef], # [EMOJI COMPATIBILITY SYMBOL-22] U+E6BF -> "[R]"
  ["ee9b80", :undef], # [EMOJI COMPATIBILITY SYMBOL-23] U+E6C0 -> "■"
  ["ee9b81", :undef], # [EMOJI COMPATIBILITY SYMBOL-24] U+E6C1 -> "■"
  ["ee9b82", :undef], # [EMOJI COMPATIBILITY SYMBOL-25] U+E6C2 -> "▼"
  ["ee9b83", :undef], # [EMOJI COMPATIBILITY SYMBOL-26] U+E6C3 -> "††††"
  ["ee9b84", :undef], # [EMOJI COMPATIBILITY SYMBOL-27] U+E6C4 -> "†††"
  ["ee9b85", :undef], # [EMOJI COMPATIBILITY SYMBOL-28] U+E6C5 -> "††"
  ["ee9b86", :undef], # [EMOJI COMPATIBILITY SYMBOL-29] U+E6C6 -> "†"
  ["ee9b87", :undef], # [EMOJI COMPATIBILITY SYMBOL-30] U+E6C7 -> "[I]"
  ["ee9b88", :undef], # [EMOJI COMPATIBILITY SYMBOL-31] U+E6C8 -> "[M]"
  ["ee9b89", :undef], # [EMOJI COMPATIBILITY SYMBOL-32] U+E6C9 -> "[E]"
  ["ee9b8a", :undef], # [EMOJI COMPATIBILITY SYMBOL-33] U+E6CA -> "[VE]"
  ["ee9b8b", :undef], # [EMOJI COMPATIBILITY SYMBOL-34] U+E6CB -> "●"
  ["ee9b8c", :undef], # [EMOJI COMPATIBILITY SYMBOL-35] U+E6CC -> "[カード使用不可]"
  ["ee9b8d", :undef], # [EMOJI COMPATIBILITY SYMBOL-36] U+E6CD -> "[チェックボックス]"
  ["ee9ba1", :undef], # [EMOJI COMPATIBILITY SYMBOL-66] U+E6E1 -> "[Q]"
]

EMOJI_EXCHANGE_TBL['UTF8-KDDI']['UTF8-DoCoMo'] = [
  # for documented codepoints
  ["ee9288", "ee98be"], # [BLACK SUN WITH RAYS] U+E488 -> U+E63E
  ["ee928d", "ee98bf"], # [CLOUD] U+E48D -> U+E63F
  ["ee928c", "ee9980"], # [UMBRELLA WITH RAIN DROPS] U+E48C -> U+E640
  ["ee9285", "ee9981"], # [SNOWMAN WITHOUT SNOW] U+E485 -> U+E641
  ["ee9287", "ee9982"], # [HIGH VOLTAGE SIGN] U+E487 -> U+E642
  ["ee91a9", "ee9983"], # [CYCLONE] U+E469 -> U+E643
  ["ee9698", "ee9984"], # [FOGGY] U+E598 -> U+E644
  ["eeaba8", "ee9985"], # [CLOSED UMBRELLA] U+EAE8 -> U+E645
  ["eeabb1", "ee9ab3"], # [NIGHT WITH STARS] U+EAF1 -> U+E6B3
  ["eeabb4", "ee98be"], # [SUNRISE] U+EAF4 -> U+E63E
  ["ee979a", :undef], # [CITYSCAPE AT DUSK] U+E5DA -> "[夕焼け]"
  ["eeabb2", :undef], # [RAINBOW] U+EAF2 -> "[虹]"
  ["ee928a", :undef], # [SNOWFLAKE] U+E48A -> "[雪結晶]"
  ["ee928e", "ee98beee98bf"], # [SUN BEHIND CLOUD] U+E48E -> U+E63E U+E63F
  ["ee92bf", "ee9ab3"], # [BRIDGE AT NIGHT] U+E4BF -> U+E6B3
  ["eeadbc", "ee9cbf"], # [WATER WAVE] U+EB7C -> U+E73F
  ["eead93", :undef], # [VOLCANO] U+EB53 -> "[火山]"
  ["eead9f", "ee9ab3"], # [MILKY WAY] U+EB5F -> U+E6B3
  ["ee96b3", :undef], # [EARTH GLOBE ASIA-AUSTRALIA] U+E5B3 -> "[地球]"
  ["ee96a8", "ee9a9c"], # [NEW MOON SYMBOL] U+E5A8 -> U+E69C
  ["ee96a9", "ee9a9d"], # [WAXING GIBBOUS MOON SYMBOL] U+E5A9 -> U+E69D
  ["ee96aa", "ee9a9e"], # [FIRST QUARTER MOON SYMBOL] U+E5AA -> U+E69E
  ["ee9286", "ee9a9f"], # [CRESCENT MOON] U+E486 -> U+E69F
  ["ee9289", "ee9a9e"], # [FIRST QUARTER MOON WITH FACE] U+E489 -> U+E69E
  ["ee91a8", :undef], # [SHOOTING STAR] U+E468 -> "☆彡"
  ["ee95ba", "ee9c9f"], # [WATCH] U+E57A -> U+E71F
  ["ee95bb", "ee9c9c"], # [HOURGLASS] U+E57B -> U+E71C
  ["ee9694", "ee9aba"], # [ALARM CLOCK] U+E594 -> U+E6BA
  ["ee91bc", "ee9c9c"], # [HOURGLASS WITH FLOWING SAND] U+E47C -> U+E71C
  ["ee928f", "ee9986"], # [ARIES] U+E48F -> U+E646
  ["ee9290", "ee9987"], # [TAURUS] U+E490 -> U+E647
  ["ee9291", "ee9988"], # [GEMINI] U+E491 -> U+E648
  ["ee9292", "ee9989"], # [CANCER] U+E492 -> U+E649
  ["ee9293", "ee998a"], # [LEO] U+E493 -> U+E64A
  ["ee9294", "ee998b"], # [VIRGO] U+E494 -> U+E64B
  ["ee9295", "ee998c"], # [LIBRA] U+E495 -> U+E64C
  ["ee9296", "ee998d"], # [SCORPIUS] U+E496 -> U+E64D
  ["ee9297", "ee998e"], # [SAGITTARIUS] U+E497 -> U+E64E
  ["ee9298", "ee998f"], # [CAPRICORN] U+E498 -> U+E64F
  ["ee9299", "ee9990"], # [AQUARIUS] U+E499 -> U+E650
  ["ee929a", "ee9991"], # [PISCES] U+E49A -> U+E651
  ["ee929b", :undef], # [OPHIUCHUS] U+E49B -> "[蛇使座]"
  ["ee9493", "ee9d81"], # [FOUR LEAF CLOVER] U+E513 -> U+E741
  ["ee93a4", "ee9d83"], # [TULIP] U+E4E4 -> U+E743
  ["eeadbd", "ee9d86"], # [SEEDLING] U+EB7D -> U+E746
  ["ee938e", "ee9d87"], # [MAPLE LEAF] U+E4CE -> U+E747
  ["ee938a", "ee9d88"], # [CHERRY BLOSSOM] U+E4CA -> U+E748
  ["ee96ba", :undef], # [ROSE] U+E5BA -> "[バラ]"
  ["ee978d", "ee9d87"], # [FALLEN LEAF] U+E5CD -> U+E747
  ["eeaa94", :undef], # [HIBISCUS] U+EA94 -> "[ハイビスカス]"
  ["ee93a3", :undef], # [SUNFLOWER] U+E4E3 -> "[ひまわり]"
  ["ee93a2", :undef], # [PALM TREE] U+E4E2 -> "[ヤシ]"
  ["eeaa96", :undef], # [CACTUS] U+EA96 -> "[サボテン]"
  ["eeacb6", :undef], # [EAR OF MAIZE] U+EB36 -> "[とうもろこし]"
  ["eeacb7", :undef], # [MUSHROOM] U+EB37 -> "[キノコ]"
  ["eeacb8", :undef], # [CHESTNUT] U+EB38 -> "[栗]"
  ["eead89", :undef], # [BLOSSOM] U+EB49 -> "[花]"
  ["eeae82", "ee9d81"], # [HERB] U+EB82 -> U+E741
  ["ee9392", "ee9d82"], # [CHERRIES] U+E4D2 -> U+E742
  ["eeacb5", "ee9d84"], # [BANANA] U+EB35 -> U+E744
  ["eeaab9", "ee9d85"], # [RED APPLE] U+EAB9 -> U+E745
  ["eeaaba", :undef], # [TANGERINE] U+EABA -> "[みかん]"
  ["ee9394", :undef], # [STRAWBERRY] U+E4D4 -> "[イチゴ]"
  ["ee938d", :undef], # [WATERMELON] U+E4CD -> "[スイカ]"
  ["eeaabb", :undef], # [TOMATO] U+EABB -> "[トマト]"
  ["eeaabc", :undef], # [AUBERGINE] U+EABC -> "[ナス]"
  ["eeacb2", :undef], # [MELON] U+EB32 -> "[メロン]"
  ["eeacb3", :undef], # [PINEAPPLE] U+EB33 -> "[パイナップル]"
  ["eeacb4", :undef], # [GRAPES] U+EB34 -> "[ブドウ]"
  ["eeacb9", :undef], # [PEACH] U+EB39 -> "[モモ]"
  ["eead9a", "ee9d85"], # [GREEN APPLE] U+EB5A -> U+E745
  ["ee96a4", "ee9a91"], # [EYES] U+E5A4 -> U+E691
  ["ee96a5", "ee9a92"], # [EAR] U+E5A5 -> U+E692
  ["eeab90", :undef], # [NOSE] U+EAD0 -> "[鼻]"
  ["eeab91", "ee9bb9"], # [MOUTH] U+EAD1 -> U+E6F9
  ["eead87", "ee9ca8"], # [TONGUE] U+EB47 -> U+E728
  ["ee9489", "ee9c90"], # [LIPSTICK] U+E509 -> U+E710
  ["eeaaa0", :undef], # [NAIL POLISH] U+EAA0 -> "[マニキュア]"
  ["ee948b", :undef], # [FACE MASSAGE] U+E50B -> "[エステ]"
  ["eeaaa1", "ee99b5"], # [HAIRCUT] U+EAA1 -> U+E675
  ["eeaaa2", :undef], # [BARBER POLE] U+EAA2 -> "[床屋]"
  ["ee93bc", "ee9bb0"], # [BOY] U+E4FC -> U+E6F0
  ["ee93ba", "ee9bb0"], # [GIRL] U+E4FA -> U+E6F0
  ["ee9481", :undef], # [FAMILY] U+E501 -> "[家族]"
  ["ee979d", :undef], # [POLICE OFFICER] U+E5DD -> "[警官]"
  ["eeab9b", :undef], # [WOMAN WITH BUNNY EARS] U+EADB -> "[バニー]"
  ["eeaba9", :undef], # [BRIDE WITH VEIL] U+EAE9 -> "[花嫁]"
  ["eeac93", :undef], # [WESTERN PERSON] U+EB13 -> "[白人]"
  ["eeac94", :undef], # [MAN WITH GUA PI MAO] U+EB14 -> "[中国人]"
  ["eeac95", :undef], # [MAN WITH TURBAN] U+EB15 -> "[インド人]"
  ["eeac96", :undef], # [OLDER MAN] U+EB16 -> "[おじいさん]"
  ["eeac97", :undef], # [OLDER WOMAN] U+EB17 -> "[おばあさん]"
  ["eeac98", :undef], # [BABY] U+EB18 -> "[赤ちゃん]"
  ["eeac99", :undef], # [CONSTRUCTION WORKER] U+EB19 -> "[工事現場の人]"
  ["eeac9a", :undef], # [PRINCESS] U+EB1A -> "[お姫様]"
  ["eead84", :undef], # [JAPANESE OGRE] U+EB44 -> "[なまはげ]"
  ["eead85", :undef], # [JAPANESE GOBLIN] U+EB45 -> "[天狗]"
  ["ee938b", :undef], # [GHOST] U+E4CB -> "[お化け]"
  ["ee96bf", :undef], # [BABY ANGEL] U+E5BF -> "[天使]"
  ["ee948e", :undef], # [EXTRATERRESTRIAL ALIEN] U+E50E -> "[UFO]"
  ["ee93ac", :undef], # [ALIEN MONSTER] U+E4EC -> "[宇宙人]"
  ["ee93af", :undef], # [IMP] U+E4EF -> "[アクマ]"
  ["ee93b8", :undef], # [SKULL] U+E4F8 -> "[ドクロ]"
  ["eeac9c", :undef], # [DANCER] U+EB1C -> "[ダンス]"
  ["eeadbe", "ee9d8e"], # [SNAIL] U+EB7E -> U+E74E
  ["eeaca2", :undef], # [SNAKE] U+EB22 -> "[ヘビ]"
  ["eeaca3", :undef], # [CHICKEN] U+EB23 -> "[ニワトリ]"
  ["eeaca4", :undef], # [BOAR] U+EB24 -> "[イノシシ]"
  ["eeaca5", :undef], # [BACTRIAN CAMEL] U+EB25 -> "[ラクダ]"
  ["eeac9f", :undef], # [ELEPHANT] U+EB1F -> "[ゾウ]"
  ["eeaca0", :undef], # [KOALA] U+EB20 -> "[コアラ]"
  ["ee9787", :undef], # [OCTOPUS] U+E5C7 -> "[タコ]"
  ["eeabac", :undef], # [SPIRAL SHELL] U+EAEC -> "[巻貝]"
  ["eeac9e", :undef], # [BUG] U+EB1E -> "[ゲジゲジ]"
  ["ee939d", :undef], # [ANT] U+E4DD -> "[アリ]"
  ["eead97", :undef], # [HONEYBEE] U+EB57 -> "[ミツバチ]"
  ["eead98", :undef], # [LADY BEETLE] U+EB58 -> "[てんとう虫]"
  ["eeac9d", "ee9d91"], # [TROPICAL FISH] U+EB1D -> U+E751
  ["ee9393", "ee9d91"], # [BLOWFISH] U+E4D3 -> U+E751
  ["ee9794", :undef], # [TURTLE] U+E5D4 -> "[カメ]"
  ["ee93a0", "ee9d8f"], # [BABY CHICK] U+E4E0 -> U+E74F
  ["eeadb6", "ee9d8f"], # [FRONT-FACING BABY CHICK] U+EB76 -> U+E74F
  ["ee979b", "ee9d8f"], # [HATCHING CHICK] U+E5DB -> U+E74F
  ["ee939c", "ee9d90"], # [PENGUIN] U+E4DC -> U+E750
  ["ee939f", "ee9aa1"], # [POODLE] U+E4DF -> U+E6A1
  ["eeac9b", :undef], # [DOLPHIN] U+EB1B -> "[イルカ]"
  ["ee9782", :undef], # [MOUSE FACE] U+E5C2 -> "[ネズミ]"
  ["ee9780", :undef], # [TIGER FACE] U+E5C0 -> "[トラ]"
  ["ee939b", "ee9aa2"], # [CAT FACE] U+E4DB -> U+E6A2
  ["ee91b0", :undef], # [SPOUTING WHALE] U+E470 -> "[クジラ]"
  ["ee9398", "ee9d94"], # [HORSE FACE] U+E4D8 -> U+E754
  ["ee9399", :undef], # [MONKEY FACE] U+E4D9 -> "[サル]"
  ["ee93a1", "ee9aa1"], # [DOG FACE] U+E4E1 -> U+E6A1
  ["ee939e", "ee9d95"], # [PIG FACE] U+E4DE -> U+E755
  ["ee9781", :undef], # [BEAR FACE] U+E5C1 -> "[クマ]"
  ["eeaca1", :undef], # [COW FACE] U+EB21 -> "[牛]"
  ["ee9397", :undef], # [RABBIT FACE] U+E4D7 -> "[ウサギ]"
  ["ee939a", :undef], # [FROG FACE] U+E4DA -> "[カエル]"
  ["ee93ae", "ee9a98"], # [PAW PRINTS] U+E4EE -> U+E698
  ["eeacbf", :undef], # [DRAGON FACE] U+EB3F -> "[辰]"
  ["eead86", :undef], # [PANDA FACE] U+EB46 -> "[パンダ]"
  ["eead88", "ee9d95"], # [PIG NOSE] U+EB48 -> U+E755
  ["ee91b2", "ee9bb1"], # [ANGRY FACE] U+E472 -> U+E6F1
  ["eeada7", "ee9bb3"], # [ANGUISHED FACE] U+EB67 -> U+E6F3
  ["eeab8a", "ee9bb4"], # [ASTONISHED FACE] U+EACA -> U+E6F4
  ["ee96ae", "ee9bb4"], # [DIZZY FACE] U+E5AE -> U+E6F4
  ["eeab8b", "ee9ca3"], # [EXASPERATED FACE] U+EACB -> U+E723
  ["eeab89", "ee9ca5"], # [EXPRESSIONLESS FACE] U+EAC9 -> U+E725
  ["ee9784", "ee9ca6"], # [FACE WITH HEART-SHAPED EYES] U+E5C4 -> U+E726
  ["eeab81", "ee9d93"], # [FACE WITH LOOK OF TRIUMPH] U+EAC1 -> U+E753
  ["ee93a7", "ee9ca8"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E4E7 -> U+E728
  ["eeab8f", "ee9ca6"], # [FACE THROWING A KISS] U+EACF -> U+E726
  ["eeab8e", "ee9ca6"], # [FACE KISSING] U+EACE -> U+E726
  ["eeab87", :undef], # [FACE WITH MASK] U+EAC7 -> "[風邪ひき]"
  ["eeab88", "ee9caa"], # [FLUSHED FACE] U+EAC8 -> U+E72A
  ["ee91b1", "ee9bb0"], # [HAPPY FACE WITH OPEN MOUTH] U+E471 -> U+E6F0
  ["eeae80", "ee9d93"], # [HAPPY FACE WITH GRIN] U+EB80 -> U+E753
  ["eeada4", "ee9caa"], # [HAPPY AND CRYING FACE] U+EB64 -> U+E72A
  ["eeab8d", "ee9bb0"], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+EACD -> U+E6F0
  ["ee93bb", "ee9bb0"], # [WHITE SMILING FACE] U+E4FB -> U+E6F0
  ["eeada9", "ee9cae"], # [CRYING FACE] U+EB69 -> U+E72E
  ["ee91b3", "ee9cad"], # [LOUDLY CRYING FACE] U+E473 -> U+E72D
  ["eeab86", "ee9d97"], # [FEARFUL FACE] U+EAC6 -> U+E757
  ["eeab82", "ee9cab"], # [PERSEVERING FACE] U+EAC2 -> U+E72B
  ["eead9d", "ee9ca4"], # [POUTING FACE] U+EB5D -> U+E724
  ["eeab85", "ee9ca1"], # [RELIEVED FACE] U+EAC5 -> U+E721
  ["eeab83", "ee9bb3"], # [CONFOUNDED FACE] U+EAC3 -> U+E6F3
  ["eeab80", "ee9ca0"], # [PENSIVE FACE] U+EAC0 -> U+E720
  ["ee9785", "ee9d97"], # [FACE SCREAMING IN FEAR] U+E5C5 -> U+E757
  ["eeab84", "ee9c81"], # [SLEEPY FACE] U+EAC4 -> U+E701
  ["eeaabf", "ee9cac"], # [SMIRKING FACE] U+EABF -> U+E72C
  ["ee9786", "ee9ca3"], # [FACE WITH COLD SWEAT] U+E5C6 -> U+E723
  ["ee91b4", "ee9cab"], # [TIRED FACE] U+E474 -> U+E72B
  ["ee9783", "ee9ca9"], # [WINKING FACE] U+E5C3 -> U+E729
  ["eeada1", "ee9bb0"], # [CAT FACE WITH OPEN MOUTH] U+EB61 -> U+E6F0
  ["eeadbf", "ee9d93"], # [HAPPY CAT FACE WITH GRIN] U+EB7F -> U+E753
  ["eeada3", "ee9caa"], # [HAPPY AND CRYING CAT FACE] U+EB63 -> U+E72A
  ["eeada0", "ee9ca6"], # [CAT FACE KISSING] U+EB60 -> U+E726
  ["eeada5", "ee9ca6"], # [CAT FACE WITH HEART-SHAPED EYES] U+EB65 -> U+E726
  ["eeada8", "ee9cae"], # [CRYING CAT FACE] U+EB68 -> U+E72E
  ["eead9e", "ee9ca4"], # [POUTING CAT FACE] U+EB5E -> U+E724
  ["eeadaa", "ee9d93"], # [CAT FACE WITH TIGHTLY-CLOSED LIPS] U+EB6A -> U+E753
  ["eeada6", "ee9bb3"], # [ANGUISHED CAT FACE] U+EB66 -> U+E6F3
  ["eeab97", "ee9caf"], # [FACE WITH NO GOOD GESTURE] U+EAD7 -> U+E72F
  ["eeab98", "ee9c8b"], # [FACE WITH OK GESTURE] U+EAD8 -> U+E70B
  ["eeab99", :undef], # [PERSON BOWING DEEPLY] U+EAD9 -> "m(_ _)m"
  ["eead90", :undef], # [SEE-NO-EVIL MONKEY] U+EB50 -> "(/_＼)"
  ["eead91", :undef], # [SPEAK-NO-EVIL MONKEY] U+EB51 -> "(・×・)"
  ["eead92", :undef], # [HEAR-NO-EVIL MONKEY] U+EB52 -> "|(・×・)|"
  ["eeae85", :undef], # [PERSON RAISING ONE HAND] U+EB85 -> "(^-^)/"
  ["eeae86", :undef], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+EB86 -> "＼(^o^)／"
  ["eeae87", "ee9bb3"], # [PERSON FROWNING] U+EB87 -> U+E6F3
  ["eeae88", "ee9bb1"], # [PERSON WITH POUTING FACE] U+EB88 -> U+E6F1
  ["eeab92", :undef], # [PERSON WITH FOLDED HANDS] U+EAD2 -> "(&gt;人&lt;)"
  ["ee92ab", "ee99a3"], # [HOUSE BUILDING] U+E4AB -> U+E663
  ["eeac89", "ee99a3"], # [HOUSE WITH GARDEN] U+EB09 -> U+E663
  ["ee92ad", "ee99a4"], # [OFFICE BUILDING] U+E4AD -> U+E664
  ["ee979e", "ee99a5"], # [JAPANESE POST OFFICE] U+E5DE -> U+E665
  ["ee979f", "ee99a6"], # [HOSPITAL] U+E5DF -> U+E666
  ["ee92aa", "ee99a7"], # [BANK] U+E4AA -> U+E667
  ["ee92a3", "ee99a8"], # [AUTOMATED TELLER MACHINE] U+E4A3 -> U+E668
  ["eeaa81", "ee99a9"], # [HOTEL] U+EA81 -> U+E669
  ["eeabb3", "ee99a9ee9baf"], # [LOVE HOTEL] U+EAF3 -> U+E669 U+E6EF
  ["ee92a4", "ee99aa"], # [CONVENIENCE STORE] U+E4A4 -> U+E66A
  ["eeaa80", "ee9cbe"], # [SCHOOL] U+EA80 -> U+E73E
  ["ee96bb", :undef], # [CHURCH] U+E5BB -> "[教会]"
  ["ee978f", :undef], # [FOUNTAIN] U+E5CF -> "[噴水]"
  ["eeabb6", :undef], # [DEPARTMENT STORE] U+EAF6 -> "[デパート]"
  ["eeabb7", :undef], # [JAPANESE CASTLE] U+EAF7 -> "[城]"
  ["eeabb8", :undef], # [EUROPEAN CASTLE] U+EAF8 -> "[城]"
  ["eeabb9", :undef], # [FACTORY] U+EAF9 -> "[工場]"
  ["ee92a9", "ee99a1"], # [ANCHOR] U+E4A9 -> U+E661
  ["ee92bd", "ee9d8b"], # [IZAKAYA LANTERN] U+E4BD -> U+E74B
  ["ee96bd", "ee9d80"], # [MOUNT FUJI] U+E5BD -> U+E740
  ["ee9380", :undef], # [TOKYO TOWER] U+E4C0 -> "[東京タワー]"
  ["ee95b2", :undef], # [SILHOUETTE OF JAPAN] U+E572 -> "[日本地図]"
  ["eeadac", :undef], # [MOYAI] U+EB6C -> "[モアイ]"
  ["ee96b7", "ee9a99"], # [MANS SHOE] U+E5B7 -> U+E699
  ["eeacab", "ee9a99"], # [ATHLETIC SHOE] U+EB2B -> U+E699
  ["ee949a", "ee99b4"], # [HIGH-HEELED SHOE] U+E51A -> U+E674
  ["eeaa9f", :undef], # [WOMANS BOOTS] U+EA9F -> "[ブーツ]"
  ["eeacaa", "ee9a98"], # [FOOTPRINTS] U+EB2A -> U+E698
  ["ee93be", "ee9a9a"], # [EYEGLASSES] U+E4FE -> U+E69A
  ["ee96b6", "ee9c8e"], # [T-SHIRT] U+E5B6 -> U+E70E
  ["eeadb7", "ee9c91"], # [JEANS] U+EB77 -> U+E711
  ["ee9789", "ee9c9a"], # [CROWN] U+E5C9 -> U+E71A
  ["eeaa93", :undef], # [NECKTIE] U+EA93 -> "[ネクタイ]"
  ["eeaa9e", :undef], # [WOMANS HAT] U+EA9E -> "[帽子]"
  ["eeadab", :undef], # [DRESS] U+EB6B -> "[ドレス]"
  ["eeaaa3", :undef], # [KIMONO] U+EAA3 -> "[着物]"
  ["eeaaa4", :undef], # [BIKINI] U+EAA4 -> "[ビキニ]"
  ["ee948d", "ee9c8e"], # [WOMANS CLOTHES] U+E50D -> U+E70E
  ["ee9484", "ee9c8f"], # [PURSE] U+E504 -> U+E70F
  ["ee929c", "ee9a82"], # [HANDBAG] U+E49C -> U+E682
  ["ee9387", "ee9c95"], # [MONEY BAG] U+E4C7 -> U+E715
  ["ee979c", :undef], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+E5DC -> "[株価]"
  ["ee95b9", "ee9c95"], # [HEAVY DOLLAR SIGN] U+E579 -> U+E715
  ["ee95bc", :undef], # [CREDIT CARD] U+E57C -> "[カード]"
  ["ee95bd", "ee9b96"], # [BANKNOTE WITH YEN SIGN] U+E57D -> U+E6D6
  ["ee9685", "ee9c95"], # [BANKNOTE WITH DOLLAR SIGN] U+E585 -> U+E715
  ["eead9b", :undef], # [MONEY WITH WINGS] U+EB5B -> "[飛んでいくお金]"
  ["eeac91", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS CN] U+EB11 -> "[中国]"
  ["eeac8e", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS DE] U+EB0E -> "[ドイツ]"
  ["ee9795", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS ES] U+E5D5 -> "[スペイン]"
  ["eeabba", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS FR] U+EAFA -> "[フランス]"
  ["eeac90", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS GB] U+EB10 -> "[イギリス]"
  ["eeac8f", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS IT] U+EB0F -> "[イタリア]"
  ["ee938c", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS JP] U+E4CC -> "[日の丸]"
  ["eeac92", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS KR] U+EB12 -> "[韓国]"
  ["ee9796", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS RU] U+E5D6 -> "[ロシア]"
  ["ee95b3", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS US] U+E573 -> "[USA]"
  ["ee91bb", :undef], # [FIRE] U+E47B -> "[炎]"
  ["ee9683", "ee9bbb"], # [ELECTRIC TORCH] U+E583 -> U+E6FB
  ["ee9687", "ee9c98"], # [WRENCH] U+E587 -> U+E718
  ["ee978b", :undef], # [HAMMER] U+E5CB -> "[ハンマー]"
  ["ee9681", :undef], # [NUT AND BOLT] U+E581 -> "[ネジ]"
  ["ee95bf", :undef], # [HOCHO] U+E57F -> "[包丁]"
  ["ee948a", :undef], # [PISTOL] U+E50A -> "[ピストル]"
  ["eeaa8f", :undef], # [CRYSTAL BALL] U+EA8F -> "[占い]"
  ["ee9280", :undef], # [JAPANESE SYMBOL FOR BEGINNER] U+E480 -> "[若葉マーク]"
  ["ee9490", :undef], # [SYRINGE] U+E510 -> "[注射]"
  ["eeaa9a", :undef], # [PILL] U+EA9A -> "[薬]"
  ["eeaca6", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+EB26 -> "[A]"
  ["eeaca7", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+EB27 -> "[B]"
  ["eeaca9", :undef], # [NEGATIVE SQUARED AB] U+EB29 -> "[AB]"
  ["eeaca8", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+EB28 -> "[O]"
  ["ee969f", "ee9a84"], # [RIBBON] U+E59F -> U+E684
  ["ee938f", "ee9a85"], # [WRAPPED PRESENT] U+E4CF -> U+E685
  ["ee96a0", "ee9a86"], # [BIRTHDAY CAKE] U+E5A0 -> U+E686
  ["ee9389", "ee9aa4"], # [CHRISTMAS TREE] U+E4C9 -> U+E6A4
  ["eeabb0", :undef], # [FATHER CHRISTMAS] U+EAF0 -> "[サンタ]"
  ["ee9799", :undef], # [CROSSED FLAGS] U+E5D9 -> "[祝日]"
  ["ee978c", :undef], # [FIREWORKS] U+E5CC -> "[花火]"
  ["eeaa9b", :undef], # [BALLOON] U+EA9B -> "[風船]"
  ["eeaa9c", :undef], # [PARTY POPPER] U+EA9C -> "[クラッカー]"
  ["eeaba3", :undef], # [PINE DECORATION] U+EAE3 -> "[門松]"
  ["eeaba4", :undef], # [JAPANESE DOLLS] U+EAE4 -> "[ひな祭り]"
  ["eeaba5", :undef], # [GRADUATION CAP] U+EAE5 -> "[卒業式]"
  ["eeaba6", :undef], # [SCHOOL SATCHEL] U+EAE6 -> "[ランドセル]"
  ["eeaba7", :undef], # [CARP STREAMER] U+EAE7 -> "[こいのぼり]"
  ["eeabab", :undef], # [FIREWORK SPARKLER] U+EAEB -> "[線香花火]"
  ["eeabad", :undef], # [WIND CHIME] U+EAED -> "[風鈴]"
  ["eeabae", :undef], # [JACK-O-LANTERN] U+EAEE -> "[ハロウィン]"
  ["ee91af", :undef], # [CONFETTI BALL] U+E46F -> "[オメデトウ]"
  ["eeacbd", :undef], # [TANABATA TREE] U+EB3D -> "[七夕]"
  ["eeabaf", :undef], # [MOON VIEWING CEREMONY] U+EAEF -> "[お月見]"
  ["ee969b", "ee999a"], # [PAGER] U+E59B -> U+E65A
  ["ee9696", "ee9a87"], # [BLACK TELEPHONE] U+E596 -> U+E687
  ["ee949e", "ee9a87"], # [TELEPHONE RECEIVER] U+E51E -> U+E687
  ["ee9688", "ee9a88"], # [MOBILE PHONE] U+E588 -> U+E688
  ["eeac88", "ee9b8e"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+EB08 -> U+E6CE
  ["eeaa92", "ee9a89"], # [MEMO] U+EA92 -> U+E689
  ["ee94a0", "ee9b90"], # [FAX MACHINE] U+E520 -> U+E6D0
  ["ee94a1", "ee9b93"], # [ENVELOPE] U+E521 -> U+E6D3
  ["ee9691", "ee9b8f"], # [INCOMING ENVELOPE] U+E591 -> U+E6CF
  ["eeada2", "ee9b8f"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+EB62 -> U+E6CF
  ["ee949b", "ee99a5"], # [CLOSED MAILBOX WITH LOWERED FLAG] U+E51B -> U+E665
  ["eeac8a", "ee99a5"], # [CLOSED MAILBOX WITH RAISED FLAG] U+EB0A -> U+E665
  ["ee968b", :undef], # [NEWSPAPER] U+E58B -> "[新聞]"
  ["ee92a8", :undef], # [SATELLITE ANTENNA] U+E4A8 -> "[アンテナ]"
  ["ee9692", :undef], # [OUTBOX TRAY] U+E592 -> "[送信BOX]"
  ["ee9693", :undef], # [INBOX TRAY] U+E593 -> "[受信BOX]"
  ["ee949f", "ee9a85"], # [PACKAGE] U+E51F -> U+E685
  ["eeadb1", "ee9b93"], # [E-MAIL SYMBOL] U+EB71 -> U+E6D3
  ["eeabbd", :undef], # [INPUT SYMBOL FOR LATIN CAPITAL LETTERS] U+EAFD -> "[ABCD]"
  ["eeabbe", :undef], # [INPUT SYMBOL FOR LATIN SMALL LETTERS] U+EAFE -> "[abcd]"
  ["eeabbf", :undef], # [INPUT SYMBOL FOR NUMBERS] U+EAFF -> "[1234]"
  ["eeac80", :undef], # [INPUT SYMBOL FOR SYMBOLS] U+EB00 -> "[記号]"
  ["eead95", :undef], # [INPUT SYMBOL FOR LATIN LETTERS] U+EB55 -> "[ABC]"
  ["eeac83", "ee9aae"], # [BLACK NIB] U+EB03 -> U+E6AE
  ["ee96b8", "ee9c96"], # [PERSONAL COMPUTER] U+E5B8 -> U+E716
  ["ee92a1", "ee9c99"], # [PENCIL] U+E4A1 -> U+E719
  ["ee92a0", "ee9cb0"], # [PAPERCLIP] U+E4A0 -> U+E730
  ["ee978e", "ee9a82"], # [BRIEFCASE] U+E5CE -> U+E682
  ["ee9682", :undef], # [MINIDISC] U+E582 -> "[MD]"
  ["ee95a2", :undef], # [FLOPPY DISK] U+E562 -> "[フロッピー]"
  ["ee948c", "ee9a8c"], # [OPTICAL DISC] U+E50C -> U+E68C
  ["ee9496", "ee99b5"], # [BLACK SCISSORS] U+E516 -> U+E675
  ["ee95a0", :undef], # [ROUND PUSHPIN] U+E560 -> "[画びょう]"
  ["ee95a1", "ee9a89"], # [PAGE WITH CURL] U+E561 -> U+E689
  ["ee95a9", "ee9a89"], # [PAGE FACING UP] U+E569 -> U+E689
  ["ee95a3", :undef], # [CALENDAR] U+E563 -> "[カレンダー]"
  ["ee968f", :undef], # [FILE FOLDER] U+E58F -> "[フォルダ]"
  ["ee9690", :undef], # [OPEN FILE FOLDER] U+E590 -> "[フォルダ]"
  ["ee95ab", "ee9a83"], # [NOTEBOOK] U+E56B -> U+E683
  ["ee929f", "ee9a83"], # [OPEN BOOK] U+E49F -> U+E683
  ["ee929d", "ee9a83"], # [NOTEBOOK WITH DECORATIVE COVER] U+E49D -> U+E683
  ["ee95a8", "ee9a83"], # [CLOSED BOOK] U+E568 -> U+E683
  ["ee95a5", "ee9a83"], # [GREEN BOOK] U+E565 -> U+E683
  ["ee95a6", "ee9a83"], # [BLUE BOOK] U+E566 -> U+E683
  ["ee95a7", "ee9a83"], # [ORANGE BOOK] U+E567 -> U+E683
  ["ee95af", "ee9a83"], # [BOOKS] U+E56F -> U+E683
  ["ee949d", :undef], # [NAME BADGE] U+E51D -> "[名札]"
  ["ee959f", "ee9c8a"], # [SCROLL] U+E55F -> U+E70A
  ["ee95a4", "ee9a89"], # [CLIPBOARD] U+E564 -> U+E689
  ["ee95aa", :undef], # [TEAR-OFF CALENDAR] U+E56A -> "[カレンダー]"
  ["ee95b4", :undef], # [BAR CHART] U+E574 -> "[グラフ]"
  ["ee95b5", :undef], # [CHART WITH UPWARDS TREND] U+E575 -> "[グラフ]"
  ["ee95b6", :undef], # [CHART WITH DOWNWARDS TREND] U+E576 -> "[グラフ]"
  ["ee95ac", "ee9a83"], # [CARD INDEX] U+E56C -> U+E683
  ["ee95ad", :undef], # [PUSHPIN] U+E56D -> "[画びょう]"
  ["ee95ae", "ee9a83"], # [LEDGER] U+E56E -> U+E683
  ["ee95b0", :undef], # [STRAIGHT RULER] U+E570 -> "[定規]"
  ["ee92a2", :undef], # [TRIANGULAR RULER] U+E4A2 -> "[三角定規]"
  ["eeac8b", "ee9a89"], # [BOOKMARK TABS] U+EB0B -> U+E689
  ["ee92ba", "ee9993"], # [BASEBALL] U+E4BA -> U+E653
  ["ee9699", "ee9994"], # [FLAG IN HOLE] U+E599 -> U+E654
  ["ee92b7", "ee9995"], # [TENNIS RACQUET AND BALL] U+E4B7 -> U+E655
  ["ee92b6", "ee9996"], # [SOCCER BALL] U+E4B6 -> U+E656
  ["eeaaac", "ee9997"], # [SKI AND SKI BOOT] U+EAAC -> U+E657
  ["ee969a", "ee9998"], # [BASKETBALL AND HOOP] U+E59A -> U+E658
  ["ee92b9", "ee9999"], # [CHEQUERED FLAG] U+E4B9 -> U+E659
  ["ee92b8", "ee9c92"], # [SNOWBOARDER] U+E4B8 -> U+E712
  ["ee91ab", "ee9cb3"], # [RUNNER] U+E46B -> U+E733
  ["eead81", "ee9c92"], # [SURFER] U+EB41 -> U+E712
  ["ee9793", :undef], # [TROPHY] U+E5D3 -> "[トロフィー]"
  ["ee92bb", :undef], # [AMERICAN FOOTBALL] U+E4BB -> "[フットボール]"
  ["eeab9e", :undef], # [SWIMMER] U+EADE -> "[水泳]"
  ["ee92b5", "ee999b"], # [TRAIN] U+E4B5 -> U+E65B
  ["ee96bc", "ee999c"], # [METRO] U+E5BC -> U+E65C
  ["ee92b0", "ee999d"], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+E4B0 -> U+E65D
  ["ee92b1", "ee999e"], # [AUTOMOBILE] U+E4B1 -> U+E65E
  ["ee92af", "ee99a0"], # [ONCOMING BUS] U+E4AF -> U+E660
  ["ee92a7", :undef], # [BUS STOP] U+E4A7 -> "[バス停]"
  ["eeaa82", "ee99a1"], # [SHIP] U+EA82 -> U+E661
  ["ee92b3", "ee99a2"], # [AIRPLANE] U+E4B3 -> U+E662
  ["ee92b4", "ee9aa3"], # [SAILBOAT] U+E4B4 -> U+E6A3
  ["eeadad", :undef], # [STATION] U+EB6D -> "[駅]"
  ["ee9788", :undef], # [ROCKET] U+E5C8 -> "[ロケット]"
  ["ee92b2", :undef], # [DELIVERY TRUCK] U+E4B2 -> "[トラック]"
  ["eeab9f", :undef], # [FIRE ENGINE] U+EADF -> "[消防車]"
  ["eeaba0", :undef], # [AMBULANCE] U+EAE0 -> "[救急車]"
  ["eeaba1", :undef], # [POLICE CAR] U+EAE1 -> "[パトカー]"
  ["ee95b1", "ee99ab"], # [FUEL PUMP] U+E571 -> U+E66B
  ["ee92a6", "ee99ac"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E4A6 -> U+E66C
  ["ee91aa", "ee99ad"], # [HORIZONTAL TRAFFIC LIGHT] U+E46A -> U+E66D
  ["ee9797", :undef], # [CONSTRUCTION SIGN] U+E5D7 -> "[工事中]"
  ["eeadb3", :undef], # [POLICE CARS REVOLVING LIGHT] U+EB73 -> "[パトカー]"
  ["ee92bc", "ee9bb7"], # [HOT SPRINGS] U+E4BC -> U+E6F7
  ["ee9790", :undef], # [TENT] U+E5D0 -> "[キャンプ]"
  ["ee91ad", :undef], # [FERRIS WHEEL] U+E46D -> "[観覧車]"
  ["eeaba2", :undef], # [ROLLER COASTER] U+EAE2 -> "[ジェットコースター]"
  ["eead82", "ee9d91"], # [FISHING POLE AND FISH] U+EB42 -> U+E751
  ["ee9483", "ee99b6"], # [MICROPHONE] U+E503 -> U+E676
  ["ee9497", "ee99b7"], # [MOVIE CAMERA] U+E517 -> U+E677
  ["ee9488", "ee99ba"], # [HEADPHONE] U+E508 -> U+E67A
  ["ee969c", "ee99bb"], # [ARTIST PALETTE] U+E59C -> U+E67B
  ["eeabb5", "ee99bc"], # [TOP HAT] U+EAF5 -> U+E67C
  ["ee969e", "ee99bd"], # [CIRCUS TENT] U+E59E -> U+E67D
  ["ee929e", "ee99be"], # [TICKET] U+E49E -> U+E67E
  ["ee92be", "ee9aac"], # [CLAPPER BOARD] U+E4BE -> U+E6AC
  ["ee969d", :undef], # [PERFORMING ARTS] U+E59D -> "[演劇]"
  ["ee9386", "ee9a8b"], # [VIDEO GAME] U+E4C6 -> U+E68B
  ["ee9791", :undef], # [MAHJONG TILE RED DRAGON] U+E5D1 -> "[麻雀]"
  ["ee9385", :undef], # [DIRECT HIT] U+E4C5 -> "[的中]"
  ["ee91ae", :undef], # [SLOT MACHINE] U+E46E -> "[777]"
  ["eeab9d", :undef], # [BILLIARDS] U+EADD -> "[ビリヤード]"
  ["ee9388", :undef], # [GAME DIE] U+E4C8 -> "[サイコロ]"
  ["eead83", :undef], # [BOWLING] U+EB43 -> "[ボーリング]"
  ["eeadae", :undef], # [FLOWER PLAYING CARDS] U+EB6E -> "[花札]"
  ["eeadaf", :undef], # [PLAYING CARD BLACK JOKER] U+EB6F -> "[ジョーカー]"
  ["ee96be", "ee9bb6"], # [MUSICAL NOTE] U+E5BE -> U+E6F6
  ["ee9485", "ee9bbf"], # [MULTIPLE MUSICAL NOTES] U+E505 -> U+E6FF
  ["ee9486", :undef], # [GUITAR] U+E506 -> "[ギター]"
  ["eead80", :undef], # [MUSICAL KEYBOARD] U+EB40 -> "[ピアノ]"
  ["eeab9c", :undef], # [TRUMPET] U+EADC -> "[トランペット]"
  ["ee9487", :undef], # [VIOLIN] U+E507 -> "[バイオリン]"
  ["eeab8c", "ee9bbf"], # [MUSICAL SCORE] U+EACC -> U+E6FF
  ["ee9495", "ee9a81"], # [CAMERA] U+E515 -> U+E681
  ["ee95be", "ee99b7"], # [VIDEO CAMERA] U+E57E -> U+E677
  ["ee9482", "ee9a8a"], # [TELEVISION] U+E502 -> U+E68A
  ["ee96b9", :undef], # [RADIO] U+E5B9 -> "[ラジオ]"
  ["ee9680", :undef], # [VIDEOCASSETTE] U+E580 -> "[ビデオ]"
  ["ee93ab", "ee9bb9"], # [KISS MARK] U+E4EB -> U+E6F9
  ["eeadb8", "ee9c97"], # [LOVE LETTER] U+EB78 -> U+E717
  ["ee9494", "ee9c9b"], # [RING] U+E514 -> U+E71B
  ["ee978a", "ee9bb9"], # [KISS] U+E5CA -> U+E6F9
  ["eeaa95", :undef], # [BOUQUET] U+EA95 -> "[花束]"
  ["eeab9a", "ee9bad"], # [COUPLE WITH HEART] U+EADA -> U+E6ED
  ["eeaa83", :undef], # [NO ONE UNDER EIGHTEEN SYMBOL] U+EA83 -> "[18禁]"
  ["ee9598", "ee9cb1"], # [COPYRIGHT SIGN] U+E558 -> U+E731
  ["ee9599", "ee9cb6"], # [REGISTERED SIGN] U+E559 -> U+E736
  ["ee958e", "ee9cb2"], # [TRADE MARK SIGN] U+E54E -> U+E732
  ["ee94b3", :undef], # [INFORMATION SOURCE] U+E533 -> "[ｉ]"
  ["eeae84", "ee9ba0"], # [HASH KEY] U+EB84 -> U+E6E0
  ["ee94a2", "ee9ba2"], # [KEYCAP 1] U+E522 -> U+E6E2
  ["ee94a3", "ee9ba3"], # [KEYCAP 2] U+E523 -> U+E6E3
  ["ee94a4", "ee9ba4"], # [KEYCAP 3] U+E524 -> U+E6E4
  ["ee94a5", "ee9ba5"], # [KEYCAP 4] U+E525 -> U+E6E5
  ["ee94a6", "ee9ba6"], # [KEYCAP 5] U+E526 -> U+E6E6
  ["ee94a7", "ee9ba7"], # [KEYCAP 6] U+E527 -> U+E6E7
  ["ee94a8", "ee9ba8"], # [KEYCAP 7] U+E528 -> U+E6E8
  ["ee94a9", "ee9ba9"], # [KEYCAP 8] U+E529 -> U+E6E9
  ["ee94aa", "ee9baa"], # [KEYCAP 9] U+E52A -> U+E6EA
  ["ee96ac", "ee9bab"], # [KEYCAP 0] U+E5AC -> U+E6EB
  ["ee94ab", :undef], # [KEYCAP TEN] U+E52B -> "[10]"
  ["eeaa84", :undef], # [ANTENNA WITH BARS] U+EA84 -> "[バリ3]"
  ["eeaa90", :undef], # [VIBRATION MODE] U+EA90 -> "[マナーモード]"
  ["eeaa91", :undef], # [MOBILE PHONE OFF] U+EA91 -> "[ケータイOFF]"
  ["ee9396", "ee99b3"], # [HAMBURGER] U+E4D6 -> U+E673
  ["ee9395", "ee9d89"], # [RICE BALL] U+E4D5 -> U+E749
  ["ee9390", "ee9d8a"], # [SHORTCAKE] U+E4D0 -> U+E74A
  ["ee96b4", "ee9d8c"], # [STEAMING BOWL] U+E5B4 -> U+E74C
  ["eeaaaf", "ee9d8d"], # [BREAD] U+EAAF -> U+E74D
  ["ee9391", :undef], # [COOKING] U+E4D1 -> "[フライパン]"
  ["eeaab0", :undef], # [SOFT ICE CREAM] U+EAB0 -> "[ソフトクリーム]"
  ["eeaab1", :undef], # [FRENCH FRIES] U+EAB1 -> "[ポテト]"
  ["eeaab2", :undef], # [DANGO] U+EAB2 -> "[だんご]"
  ["eeaab3", :undef], # [RICE CRACKER] U+EAB3 -> "[せんべい]"
  ["eeaab4", "ee9d8c"], # [COOKED RICE] U+EAB4 -> U+E74C
  ["eeaab5", :undef], # [SPAGHETTI] U+EAB5 -> "[パスタ]"
  ["eeaab6", :undef], # [CURRY AND RICE] U+EAB6 -> "[カレー]"
  ["eeaab7", :undef], # [ODEN] U+EAB7 -> "[おでん]"
  ["eeaab8", :undef], # [SUSHI] U+EAB8 -> "[すし]"
  ["eeaabd", :undef], # [BENTO BOX] U+EABD -> "[弁当]"
  ["eeaabe", :undef], # [POT OF FOOD] U+EABE -> "[鍋]"
  ["eeabaa", :undef], # [SHAVED ICE] U+EAEA -> "[カキ氷]"
  ["ee9384", :undef], # [MEAT ON BONE] U+E4C4 -> "[肉]"
  ["ee93ad", "ee9983"], # [FISH CAKE WITH SWIRL DESIGN] U+E4ED -> U+E643
  ["eeacba", :undef], # [ROASTED SWEET POTATO] U+EB3A -> "[やきいも]"
  ["eeacbb", :undef], # [SLICE OF PIZZA] U+EB3B -> "[ピザ]"
  ["eeacbc", :undef], # [POULTRY LEG] U+EB3C -> "[チキン]"
  ["eead8a", :undef], # [ICE CREAM] U+EB4A -> "[アイスクリーム]"
  ["eead8b", :undef], # [DOUGHNUT] U+EB4B -> "[ドーナツ]"
  ["eead8c", :undef], # [COOKIE] U+EB4C -> "[クッキー]"
  ["eead8d", :undef], # [CHOCOLATE BAR] U+EB4D -> "[チョコ]"
  ["eead8e", :undef], # [CANDY] U+EB4E -> "[キャンディ]"
  ["eead8f", :undef], # [LOLLIPOP] U+EB4F -> "[キャンディ]"
  ["eead96", :undef], # [CUSTARD] U+EB56 -> "[プリン]"
  ["eead99", :undef], # [HONEY POT] U+EB59 -> "[ハチミツ]"
  ["eeadb0", :undef], # [FRIED SHRIMP] U+EB70 -> "[エビフライ]"
  ["ee92ac", "ee99af"], # [FORK AND KNIFE] U+E4AC -> U+E66F
  ["ee9697", "ee99b0"], # [HOT BEVERAGE] U+E597 -> U+E670
  ["ee9382", "ee99b1"], # [COCKTAIL GLASS] U+E4C2 -> U+E671
  ["ee9383", "ee99b2"], # [BEER MUG] U+E4C3 -> U+E672
  ["eeaaae", "ee9c9e"], # [TEACUP WITHOUT HANDLE] U+EAAE -> U+E71E
  ["eeaa97", "ee9d8b"], # [SAKE BOTTLE AND CUP] U+EA97 -> U+E74B
  ["ee9381", "ee9d96"], # [WINE GLASS] U+E4C1 -> U+E756
  ["eeaa98", "ee99b2"], # [CLINKING BEER MUGS] U+EA98 -> U+E672
  ["eeacbe", "ee99b1"], # [TROPICAL DRINK] U+EB3E -> U+E671
  ["ee9595", "ee99b8"], # [NORTH EAST ARROW] U+E555 -> U+E678
  ["ee958d", "ee9a96"], # [SOUTH EAST ARROW] U+E54D -> U+E696
  ["ee958c", "ee9a97"], # [NORTH WEST ARROW] U+E54C -> U+E697
  ["ee9596", "ee9aa5"], # [SOUTH WEST ARROW] U+E556 -> U+E6A5
  ["eeacad", "ee9bb5"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+EB2D -> U+E6F5
  ["eeacae", "ee9c80"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+EB2E -> U+E700
  ["eeadba", "ee9cbc"], # [LEFT RIGHT ARROW] U+EB7A -> U+E73C
  ["eeadbb", "ee9cbd"], # [UP DOWN ARROW] U+EB7B -> U+E73D
  ["ee94bf", :undef], # [UPWARDS BLACK ARROW] U+E53F -> "[↑]"
  ["ee9580", :undef], # [DOWNWARDS BLACK ARROW] U+E540 -> "[↓]"
  ["ee9592", :undef], # [BLACK RIGHTWARDS ARROW] U+E552 -> "[→]"
  ["ee9593", :undef], # [LEFTWARDS BLACK ARROW] U+E553 -> "[←]"
  ["ee94ae", :undef], # [BLACK RIGHT-POINTING TRIANGLE] U+E52E -> "[&gt;]"
  ["ee94ad", :undef], # [BLACK LEFT-POINTING TRIANGLE] U+E52D -> "[&lt;]"
  ["ee94b0", :undef], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+E530 -> "[&gt;&gt;]"
  ["ee94af", :undef], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+E52F -> "[&lt;&lt;]"
  ["ee9585", :undef], # [BLACK UP-POINTING DOUBLE TRIANGLE] U+E545 -> "▲"
  ["ee9584", :undef], # [BLACK DOWN-POINTING DOUBLE TRIANGLE] U+E544 -> "▼"
  ["ee959a", :undef], # [UP-POINTING RED TRIANGLE] U+E55A -> "▲"
  ["ee959b", :undef], # [DOWN-POINTING RED TRIANGLE] U+E55B -> "▼"
  ["ee9583", :undef], # [UP-POINTING SMALL RED TRIANGLE] U+E543 -> "▲"
  ["ee9582", :undef], # [DOWN-POINTING SMALL RED TRIANGLE] U+E542 -> "▼"
  ["eeaaad", "ee9aa0"], # [HEAVY LARGE CIRCLE] U+EAAD -> U+E6A0
  ["ee9590", :undef], # [CROSS MARK] U+E550 -> "[×]"
  ["ee9591", :undef], # [NEGATIVE SQUARED CROSS MARK] U+E551 -> "[×]"
  ["ee9282", "ee9c82"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E482 -> U+E702
  ["eeacaf", "ee9c83"], # [EXCLAMATION QUESTION MARK] U+EB2F -> U+E703
  ["eeacb0", "ee9c84"], # [DOUBLE EXCLAMATION MARK] U+EB30 -> U+E704
  ["ee9283", :undef], # [BLACK QUESTION MARK ORNAMENT] U+E483 -> "[？]"
  ["eeacb1", "ee9c8a"], # [CURLY LOOP] U+EB31 -> U+E70A
  ["ee9695", "ee9bac"], # [HEAVY BLACK HEART] U+E595 -> U+E6EC
  ["eeadb5", "ee9bad"], # [BEATING HEART] U+EB75 -> U+E6ED
  ["ee91b7", "ee9bae"], # [BROKEN HEART] U+E477 -> U+E6EE
  ["ee91b8", "ee9baf"], # [TWO HEARTS] U+E478 -> U+E6EF
  ["eeaaa6", "ee9bac"], # [SPARKLING HEART] U+EAA6 -> U+E6EC
  ["ee93aa", "ee9bac"], # [HEART WITH ARROW] U+E4EA -> U+E6EC
  ["eeaaa7", "ee9bac"], # [BLUE HEART] U+EAA7 -> U+E6EC
  ["eeaaa8", "ee9bac"], # [GREEN HEART] U+EAA8 -> U+E6EC
  ["eeaaa9", "ee9bac"], # [YELLOW HEART] U+EAA9 -> U+E6EC
  ["eeaaaa", "ee9bac"], # [PURPLE HEART] U+EAAA -> U+E6EC
  ["eead94", "ee9bac"], # [HEART WITH RIBBON] U+EB54 -> U+E6EC
  ["ee96af", "ee9bad"], # [REVOLVING HEARTS] U+E5AF -> U+E6ED
  ["eeaaa5", "ee9a8d"], # [BLACK HEART SUIT] U+EAA5 -> U+E68D
  ["ee96a1", "ee9a8e"], # [BLACK SPADE SUIT] U+E5A1 -> U+E68E
  ["ee96a2", "ee9a8f"], # [BLACK DIAMOND SUIT] U+E5A2 -> U+E68F
  ["ee96a3", "ee9a90"], # [BLACK CLUB SUIT] U+E5A3 -> U+E690
  ["ee91bd", "ee99bf"], # [SMOKING SYMBOL] U+E47D -> U+E67F
  ["ee91be", "ee9a80"], # [NO SMOKING SYMBOL] U+E47E -> U+E680
  ["ee91bf", "ee9a9b"], # [WHEELCHAIR SYMBOL] U+E47F -> U+E69B
  ["eeacac", "ee9b9e"], # [TRIANGULAR FLAG ON POST] U+EB2C -> U+E6DE
  ["ee9281", "ee9cb7"], # [WARNING SIGN] U+E481 -> U+E737
  ["ee9284", "ee9caf"], # [NO ENTRY] U+E484 -> U+E72F
  ["eeadb9", "ee9cb5"], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+EB79 -> U+E735
  ["ee92ae", "ee9c9d"], # [BICYCLE] U+E4AE -> U+E71D
  ["eeadb2", "ee9cb3"], # [PEDESTRIAN] U+EB72 -> U+E733
  ["ee9798", "ee9bb7"], # [BATH] U+E5D8 -> U+E6F7
  ["ee92a5", "ee99ae"], # [RESTROOM] U+E4A5 -> U+E66E
  ["ee9581", "ee9cb8"], # [NO ENTRY SIGN] U+E541 -> U+E738
  ["ee9597", :undef], # [HEAVY CHECK MARK] U+E557 -> "[チェックマーク]"
  ["ee96ab", "ee9b9b"], # [SQUARED CL] U+E5AB -> U+E6DB
  ["eeaa85", :undef], # [SQUARED COOL] U+EA85 -> "[COOL]"
  ["ee95b8", "ee9b97"], # [SQUARED FREE] U+E578 -> U+E6D7
  ["eeaa88", "ee9b98"], # [SQUARED ID] U+EA88 -> U+E6D8
  ["ee96b5", "ee9b9d"], # [SQUARED NEW] U+E5B5 -> U+E6DD
  ["ee96ad", "ee9c8b"], # [SQUARED OK] U+E5AD -> U+E70B
  ["ee93a8", :undef], # [SQUARED SOS] U+E4E8 -> "[SOS]"
  ["ee948f", :undef], # [SQUARED UP WITH EXCLAMATION MARK] U+E50F -> "[UP!]"
  ["ee9792", :undef], # [SQUARED VS] U+E5D2 -> "[VS]"
  ["eeaa87", :undef], # [SQUARED KATAKANA SA] U+EA87 -> "[サービス]"
  ["eeaa8a", "ee9cb9"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+EA8A -> U+E739
  ["eeaa89", "ee9cbb"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+EA89 -> U+E73B
  ["eeaa86", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+EA86 -> "[割]"
  ["eeaa8b", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+EA8B -> "[指]"
  ["eeaa8c", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+EA8C -> "[営]"
  ["ee93b1", "ee9cb4"], # [CIRCLED IDEOGRAPH SECRET] U+E4F1 -> U+E734
  ["eeaa99", :undef], # [CIRCLED IDEOGRAPH CONGRATULATION] U+EA99 -> "[祝]"
  ["ee93b7", :undef], # [CIRCLED IDEOGRAPH ADVANTAGE] U+E4F7 -> "[得]"
  ["eeac81", :undef], # [CIRCLED IDEOGRAPH ACCEPT] U+EB01 -> "[可]"
  ["ee94bc", :undef], # [HEAVY PLUS SIGN] U+E53C -> "[＋]"
  ["ee94bd", :undef], # [HEAVY MINUS SIGN] U+E53D -> "[－]"
  ["ee958f", :undef], # [HEAVY MULTIPLICATION X] U+E54F -> "[×]"
  ["ee9594", :undef], # [HEAVY DIVISION SIGN] U+E554 -> "[÷]"
  ["ee91b6", "ee9bbb"], # [ELECTRIC LIGHT BULB] U+E476 -> U+E6FB
  ["ee93a5", "ee9bbc"], # [ANGER SYMBOL] U+E4E5 -> U+E6FC
  ["ee91ba", "ee9bbe"], # [BOMB] U+E47A -> U+E6FE
  ["ee91b5", "ee9c81"], # [SLEEPING SYMBOL] U+E475 -> U+E701
  ["ee96b0", "ee9c85"], # [COLLISION SYMBOL] U+E5B0 -> U+E705
  ["ee96b1", "ee9c86"], # [SPLASHING SWEAT SYMBOL] U+E5B1 -> U+E706
  ["ee93a6", "ee9c87"], # [DROP OF WATER] U+E4E6 -> U+E707
  ["ee93b4", "ee9c88"], # [DASH SYMBOL] U+E4F4 -> U+E708
  ["ee93b5", :undef], # [PILE OF POO] U+E4F5 -> "[ウンチ]"
  ["ee93a9", :undef], # [FLEXED BICEPS] U+E4E9 -> "[力こぶ]"
  ["eead9c", :undef], # [DIZZY SYMBOL] U+EB5C -> "[クラクラ]"
  ["ee93bd", :undef], # [SPEECH BALLOON] U+E4FD -> "[フキダシ]"
  ["eeaaab", "ee9bba"], # [SPARKLES] U+EAAB -> U+E6FA
  ["ee91b9", "ee9bb8"], # [EIGHT POINTED BLACK STAR] U+E479 -> U+E6F8
  ["ee94be", "ee9bb8"], # [EIGHT SPOKED ASTERISK] U+E53E -> U+E6F8
  ["ee94ba", "ee9a9c"], # [MEDIUM WHITE CIRCLE] U+E53A -> U+E69C
  ["ee94bb", "ee9a9c"], # [MEDIUM BLACK CIRCLE] U+E53B -> U+E69C
  ["ee958a", "ee9a9c"], # [LARGE RED CIRCLE] U+E54A -> U+E69C
  ["ee958b", "ee9a9c"], # [LARGE BLUE CIRCLE] U+E54B -> U+E69C
  ["ee928b", :undef], # [WHITE MEDIUM STAR] U+E48B -> "[☆]"
  ["ee9588", :undef], # [WHITE LARGE SQUARE] U+E548 -> "■"
  ["ee9589", :undef], # [BLACK LARGE SQUARE] U+E549 -> "■"
  ["ee94b1", :undef], # [WHITE SMALL SQUARE] U+E531 -> "■"
  ["ee94b2", :undef], # [BLACK SMALL SQUARE] U+E532 -> "■"
  ["ee94b4", :undef], # [WHITE MEDIUM SMALL SQUARE] U+E534 -> "■"
  ["ee94b5", :undef], # [BLACK MEDIUM SMALL SQUARE] U+E535 -> "■"
  ["ee94b8", :undef], # [WHITE MEDIUM SQUARE] U+E538 -> "■"
  ["ee94b9", :undef], # [BLACK MEDIUM SQUARE] U+E539 -> "■"
  ["ee9586", :undef], # [LARGE ORANGE DIAMOND] U+E546 -> "◆"
  ["ee9587", :undef], # [LARGE BLUE DIAMOND] U+E547 -> "◆"
  ["ee94b6", :undef], # [SMALL ORANGE DIAMOND] U+E536 -> "◆"
  ["ee94b7", :undef], # [SMALL BLUE DIAMOND] U+E537 -> "◆"
  ["ee91ac", "ee9bba"], # [SPARKLE] U+E46C -> U+E6FA
  ["ee93b0", :undef], # [WHITE FLOWER] U+E4F0 -> "[花丸]"
  ["ee93b2", :undef], # [HUNDRED POINTS SYMBOL] U+E4F2 -> "[100点]"
  ["ee959d", "ee9b9a"], # [LEFTWARDS ARROW WITH HOOK] U+E55D -> U+E6DA
  ["ee959c", :undef], # [RIGHTWARDS ARROW WITH HOOK] U+E55C -> "└→"
  ["eeac8d", "ee9cb5"], # [CLOCKWISE DOWNWARDS AND UPWARDS OPEN CIRCLE ARROWS] U+EB0D -> U+E735
  ["ee9491", :undef], # [SPEAKER WITH THREE SOUND WAVES] U+E511 -> "[スピーカ]"
  ["ee9684", :undef], # [BATTERY] U+E584 -> "[電池]"
  ["ee9689", :undef], # [ELECTRIC PLUG] U+E589 -> "[コンセント]"
  ["ee9498", "ee9b9c"], # [LEFT-POINTING MAGNIFYING GLASS] U+E518 -> U+E6DC
  ["eeac85", "ee9b9c"], # [RIGHT-POINTING MAGNIFYING GLASS] U+EB05 -> U+E6DC
  ["ee949c", "ee9b99"], # [LOCK] U+E51C -> U+E6D9
  ["eeac8c", "ee9b99"], # [LOCK WITH INK PEN] U+EB0C -> U+E6D9
  ["eeabbc", "ee9b99"], # [CLOSED LOCK WITH KEY] U+EAFC -> U+E6D9
  ["ee9499", "ee9b99"], # [KEY] U+E519 -> U+E6D9
  ["ee9492", "ee9c93"], # [BELL] U+E512 -> U+E713
  ["eeac82", :undef], # [BALLOT BOX WITH CHECK] U+EB02 -> "[チェックマーク]"
  ["eeac84", :undef], # [RADIO BUTTON] U+EB04 -> "[ラジオボタン]"
  ["eeac87", :undef], # [BOOKMARK] U+EB07 -> "[ブックマーク]"
  ["ee968a", :undef], # [LINK SYMBOL] U+E58A -> "[リンク]"
  ["eeac86", :undef], # [BACK WITH LEFTWARDS ARROW ABOVE] U+EB06 -> "[←BACK]"
  ["ee968c", :undef], # [EM SPACE] U+E58C -> U+3013 (GETA)
  ["ee968d", :undef], # [EN SPACE] U+E58D -> U+3013 (GETA)
  ["ee968e", :undef], # [FOUR-PER-EM SPACE] U+E58E -> U+3013 (GETA)
  ["ee959e", :undef], # [WHITE HEAVY CHECK MARK] U+E55E -> "[チェックマーク]"
  ["eeae83", "ee9a93"], # [RAISED FIST] U+EB83 -> U+E693
  ["ee96a7", "ee9a95"], # [RAISED HAND] U+E5A7 -> U+E695
  ["ee96a6", "ee9a94"], # [VICTORY HAND] U+E5A6 -> U+E694
  ["ee93b3", "ee9bbd"], # [FISTED HAND SIGN] U+E4F3 -> U+E6FD
  ["ee93b9", "ee9ca7"], # [THUMBS UP SIGN] U+E4F9 -> U+E727
  ["ee93b6", :undef], # [WHITE UP POINTING INDEX] U+E4F6 -> "[人差し指]"
  ["eeaa8d", :undef], # [WHITE UP POINTING BACKHAND INDEX] U+EA8D -> "[↑]"
  ["eeaa8e", :undef], # [WHITE DOWN POINTING BACKHAND INDEX] U+EA8E -> "[↓]"
  ["ee93bf", :undef], # [WHITE LEFT POINTING BACKHAND INDEX] U+E4FF -> "[←]"
  ["ee9480", :undef], # [WHITE RIGHT POINTING BACKHAND INDEX] U+E500 -> "[→]"
  ["eeab96", "ee9a95"], # [WAVING HAND SIGN] U+EAD6 -> U+E695
  ["eeab93", :undef], # [CLAPPING HANDS SIGN] U+EAD3 -> "[拍手]"
  ["eeab94", "ee9c8b"], # [OK HAND SIGN] U+EAD4 -> U+E70B
  ["eeab95", "ee9c80"], # [THUMBS DOWN SIGN] U+EAD5 -> U+E700
  ["ee95b7", :undef], # [EMOJI COMPATIBILITY SYMBOL-37] U+E577 -> "[EZ]"
  ["ee96b2", :undef], # [EMOJI COMPATIBILITY SYMBOL-38] U+E5B2 -> "[ezplus]"
  ["eeaa9d", :undef], # [EMOJI COMPATIBILITY SYMBOL-39] U+EA9D -> "[EZナビ]"
  ["eeadb4", :undef], # [EMOJI COMPATIBILITY SYMBOL-40] U+EB74 -> "[EZムービー]"
  ["eeae81", :undef], # [EMOJI COMPATIBILITY SYMBOL-41] U+EB81 -> "[Cメール]"
  ["eeae89", :undef], # [EMOJI COMPATIBILITY SYMBOL-42] U+EB89 -> "[Java]"
  ["eeae8a", :undef], # [EMOJI COMPATIBILITY SYMBOL-43] U+EB8A -> "[BREW]"
  ["eeae8b", :undef], # [EMOJI COMPATIBILITY SYMBOL-44] U+EB8B -> "[EZ着うた]"
  ["eeae8c", :undef], # [EMOJI COMPATIBILITY SYMBOL-45] U+EB8C -> "[EZナビ]"
  ["eeae8d", :undef], # [EMOJI COMPATIBILITY SYMBOL-46] U+EB8D -> "[WIN]"
  ["eeae8e", :undef], # [EMOJI COMPATIBILITY SYMBOL-47] U+EB8E -> "[プレミアム]"
  ["eeabbb", :undef], # [EMOJI COMPATIBILITY SYMBOL-48] U+EAFB -> "[オープンウェブ]"
  ["ee9686", :undef], # [EMOJI COMPATIBILITY SYMBOL-49] U+E586 -> "[PDC]"
  ["ee94ac", "ee9ba1"], # [EMOJI COMPATIBILITY SYMBOL-66] U+E52C -> U+E6E1
  # for undocumented codepoints
  ["eebda0", "ee98be"], # [BLACK SUN WITH RAYS] U+E488 -> U+E63E
  ["eebda5", "ee98bf"], # [CLOUD] U+E48D -> U+E63F
  ["eebda4", "ee9980"], # [UMBRELLA WITH RAIN DROPS] U+E48C -> U+E640
  ["eebd9d", "ee9981"], # [SNOWMAN WITHOUT SNOW] U+E485 -> U+E641
  ["eebd9f", "ee9982"], # [HIGH VOLTAGE SIGN] U+E487 -> U+E642
  ["eebd81", "ee9983"], # [CYCLONE] U+E469 -> U+E643
  ["ef82b5", "ee9984"], # [FOGGY] U+E598 -> U+E644
  ["eeb2bc", "ee9985"], # [CLOSED UMBRELLA] U+EAE8 -> U+E645
  ["eeb385", "ee9ab3"], # [NIGHT WITH STARS] U+EAF1 -> U+E6B3
  ["eeb388", "ee98be"], # [SUNRISE] U+EAF4 -> U+E63E
  ["eeb18d", :undef], # [CITYSCAPE AT DUSK] U+E5DA -> "[夕焼け]"
  ["eeb386", :undef], # [RAINBOW] U+EAF2 -> "[虹]"
  ["eebda2", :undef], # [SNOWFLAKE] U+E48A -> "[雪結晶]"
  ["eebda6", "ee98beee98bf"], # [SUN BEHIND CLOUD] U+E48E -> U+E63E U+E63F
  ["eebe98", "ee9ab3"], # [BRIDGE AT NIGHT] U+E4BF -> U+E6B3
  ["eeb681", "ee9cbf"], # [WATER WAVE] U+EB7C -> U+E73F
  ["eeb597", :undef], # [VOLCANO] U+EB53 -> "[火山]"
  ["eeb5a3", "ee9ab3"], # [MILKY WAY] U+EB5F -> U+E6B3
  ["ef8390", :undef], # [EARTH GLOBE ASIA-AUSTRALIA] U+E5B3 -> "[地球]"
  ["ef8385", "ee9a9c"], # [NEW MOON SYMBOL] U+E5A8 -> U+E69C
  ["ef8386", "ee9a9d"], # [WAXING GIBBOUS MOON SYMBOL] U+E5A9 -> U+E69D
  ["ef8387", "ee9a9e"], # [FIRST QUARTER MOON SYMBOL] U+E5AA -> U+E69E
  ["eebd9e", "ee9a9f"], # [CRESCENT MOON] U+E486 -> U+E69F
  ["eebda1", "ee9a9e"], # [FIRST QUARTER MOON WITH FACE] U+E489 -> U+E69E
  ["eebd80", :undef], # [SHOOTING STAR] U+E468 -> "☆彡"
  ["ef8297", "ee9c9f"], # [WATCH] U+E57A -> U+E71F
  ["ef8298", "ee9c9c"], # [HOURGLASS] U+E57B -> U+E71C
  ["ef82b1", "ee9aba"], # [ALARM CLOCK] U+E594 -> U+E6BA
  ["eebd94", "ee9c9c"], # [HOURGLASS WITH FLOWING SAND] U+E47C -> U+E71C
  ["eebda7", "ee9986"], # [ARIES] U+E48F -> U+E646
  ["eebda8", "ee9987"], # [TAURUS] U+E490 -> U+E647
  ["eebda9", "ee9988"], # [GEMINI] U+E491 -> U+E648
  ["eebdaa", "ee9989"], # [CANCER] U+E492 -> U+E649
  ["eebdab", "ee998a"], # [LEO] U+E493 -> U+E64A
  ["eebdac", "ee998b"], # [VIRGO] U+E494 -> U+E64B
  ["eebdad", "ee998c"], # [LIBRA] U+E495 -> U+E64C
  ["eebdae", "ee998d"], # [SCORPIUS] U+E496 -> U+E64D
  ["eebdaf", "ee998e"], # [SAGITTARIUS] U+E497 -> U+E64E
  ["eebdb0", "ee998f"], # [CAPRICORN] U+E498 -> U+E64F
  ["eebdb1", "ee9990"], # [AQUARIUS] U+E499 -> U+E650
  ["eebdb2", "ee9991"], # [PISCES] U+E49A -> U+E651
  ["eebdb3", :undef], # [OPHIUCHUS] U+E49B -> "[蛇使座]"
  ["eebfac", "ee9d81"], # [FOUR LEAF CLOVER] U+E513 -> U+E741
  ["eebebd", "ee9d83"], # [TULIP] U+E4E4 -> U+E743
  ["eeb682", "ee9d86"], # [SEEDLING] U+EB7D -> U+E746
  ["eebea7", "ee9d87"], # [MAPLE LEAF] U+E4CE -> U+E747
  ["eebea3", "ee9d88"], # [CHERRY BLOSSOM] U+E4CA -> U+E748
  ["ef83aa", :undef], # [ROSE] U+E5BA -> "[バラ]"
  ["eeb180", "ee9d87"], # [FALLEN LEAF] U+E5CD -> U+E747
  ["eeb1a7", :undef], # [HIBISCUS] U+EA94 -> "[ハイビスカス]"
  ["eebebc", :undef], # [SUNFLOWER] U+E4E3 -> "[ひまわり]"
  ["eebebb", :undef], # [PALM TREE] U+E4E2 -> "[ヤシ]"
  ["eeb1a9", :undef], # [CACTUS] U+EA96 -> "[サボテン]"
  ["eeb3b7", :undef], # [EAR OF MAIZE] U+EB36 -> "[とうもろこし]"
  ["eeb3b8", :undef], # [MUSHROOM] U+EB37 -> "[キノコ]"
  ["eeb3b9", :undef], # [CHESTNUT] U+EB38 -> "[栗]"
  ["eeb58d", :undef], # [BLOSSOM] U+EB49 -> "[花]"
  ["eeb687", "ee9d81"], # [HERB] U+EB82 -> U+E741
  ["eebeab", "ee9d82"], # [CHERRIES] U+E4D2 -> U+E742
  ["eeb3b6", "ee9d84"], # [BANANA] U+EB35 -> U+E744
  ["eeb28d", "ee9d85"], # [RED APPLE] U+EAB9 -> U+E745
  ["eeb28e", :undef], # [TANGERINE] U+EABA -> "[みかん]"
  ["eebead", :undef], # [STRAWBERRY] U+E4D4 -> "[イチゴ]"
  ["eebea6", :undef], # [WATERMELON] U+E4CD -> "[スイカ]"
  ["eeb28f", :undef], # [TOMATO] U+EABB -> "[トマト]"
  ["eeb290", :undef], # [AUBERGINE] U+EABC -> "[ナス]"
  ["eeb3b3", :undef], # [MELON] U+EB32 -> "[メロン]"
  ["eeb3b4", :undef], # [PINEAPPLE] U+EB33 -> "[パイナップル]"
  ["eeb3b5", :undef], # [GRAPES] U+EB34 -> "[ブドウ]"
  ["eeb3ba", :undef], # [PEACH] U+EB39 -> "[モモ]"
  ["eeb59e", "ee9d85"], # [GREEN APPLE] U+EB5A -> U+E745
  ["ef8381", "ee9a91"], # [EYES] U+E5A4 -> U+E691
  ["ef8382", "ee9a92"], # [EAR] U+E5A5 -> U+E692
  ["eeb2a4", :undef], # [NOSE] U+EAD0 -> "[鼻]"
  ["eeb2a5", "ee9bb9"], # [MOUTH] U+EAD1 -> U+E6F9
  ["eeb58b", "ee9ca8"], # [TONGUE] U+EB47 -> U+E728
  ["eebfa2", "ee9c90"], # [LIPSTICK] U+E509 -> U+E710
  ["eeb1b3", :undef], # [NAIL POLISH] U+EAA0 -> "[マニキュア]"
  ["eebfa4", :undef], # [FACE MASSAGE] U+E50B -> "[エステ]"
  ["eeb1b4", "ee99b5"], # [HAIRCUT] U+EAA1 -> U+E675
  ["eeb1b5", :undef], # [BARBER POLE] U+EAA2 -> "[床屋]"
  ["eebf95", "ee9bb0"], # [BOY] U+E4FC -> U+E6F0
  ["eebf93", "ee9bb0"], # [GIRL] U+E4FA -> U+E6F0
  ["eebf9a", :undef], # [FAMILY] U+E501 -> "[家族]"
  ["eeb190", :undef], # [POLICE OFFICER] U+E5DD -> "[警官]"
  ["eeb2af", :undef], # [WOMAN WITH BUNNY EARS] U+EADB -> "[バニー]"
  ["eeb2bd", :undef], # [BRIDE WITH VEIL] U+EAE9 -> "[花嫁]"
  ["eeb394", :undef], # [WESTERN PERSON] U+EB13 -> "[白人]"
  ["eeb395", :undef], # [MAN WITH GUA PI MAO] U+EB14 -> "[中国人]"
  ["eeb396", :undef], # [MAN WITH TURBAN] U+EB15 -> "[インド人]"
  ["eeb397", :undef], # [OLDER MAN] U+EB16 -> "[おじいさん]"
  ["eeb398", :undef], # [OLDER WOMAN] U+EB17 -> "[おばあさん]"
  ["eeb399", :undef], # [BABY] U+EB18 -> "[赤ちゃん]"
  ["eeb39a", :undef], # [CONSTRUCTION WORKER] U+EB19 -> "[工事現場の人]"
  ["eeb39b", :undef], # [PRINCESS] U+EB1A -> "[お姫様]"
  ["eeb588", :undef], # [JAPANESE OGRE] U+EB44 -> "[なまはげ]"
  ["eeb589", :undef], # [JAPANESE GOBLIN] U+EB45 -> "[天狗]"
  ["eebea4", :undef], # [GHOST] U+E4CB -> "[お化け]"
  ["ef83af", :undef], # [BABY ANGEL] U+E5BF -> "[天使]"
  ["eebfa7", :undef], # [EXTRATERRESTRIAL ALIEN] U+E50E -> "[UFO]"
  ["eebf85", :undef], # [ALIEN MONSTER] U+E4EC -> "[宇宙人]"
  ["eebf88", :undef], # [IMP] U+E4EF -> "[アクマ]"
  ["eebf91", :undef], # [SKULL] U+E4F8 -> "[ドクロ]"
  ["eeb39d", :undef], # [DANCER] U+EB1C -> "[ダンス]"
  ["eeb683", "ee9d8e"], # [SNAIL] U+EB7E -> U+E74E
  ["eeb3a3", :undef], # [SNAKE] U+EB22 -> "[ヘビ]"
  ["eeb3a4", :undef], # [CHICKEN] U+EB23 -> "[ニワトリ]"
  ["eeb3a5", :undef], # [BOAR] U+EB24 -> "[イノシシ]"
  ["eeb3a6", :undef], # [BACTRIAN CAMEL] U+EB25 -> "[ラクダ]"
  ["eeb3a0", :undef], # [ELEPHANT] U+EB1F -> "[ゾウ]"
  ["eeb3a1", :undef], # [KOALA] U+EB20 -> "[コアラ]"
  ["ef83b7", :undef], # [OCTOPUS] U+E5C7 -> "[タコ]"
  ["eeb380", :undef], # [SPIRAL SHELL] U+EAEC -> "[巻貝]"
  ["eeb39f", :undef], # [BUG] U+EB1E -> "[ゲジゲジ]"
  ["eebeb6", :undef], # [ANT] U+E4DD -> "[アリ]"
  ["eeb59b", :undef], # [HONEYBEE] U+EB57 -> "[ミツバチ]"
  ["eeb59c", :undef], # [LADY BEETLE] U+EB58 -> "[てんとう虫]"
  ["eeb39e", "ee9d91"], # [TROPICAL FISH] U+EB1D -> U+E751
  ["eebeac", "ee9d91"], # [BLOWFISH] U+E4D3 -> U+E751
  ["eeb187", :undef], # [TURTLE] U+E5D4 -> "[カメ]"
  ["eebeb9", "ee9d8f"], # [BABY CHICK] U+E4E0 -> U+E74F
  ["eeb5ba", "ee9d8f"], # [FRONT-FACING BABY CHICK] U+EB76 -> U+E74F
  ["eeb18e", "ee9d8f"], # [HATCHING CHICK] U+E5DB -> U+E74F
  ["eebeb5", "ee9d90"], # [PENGUIN] U+E4DC -> U+E750
  ["eebeb8", "ee9aa1"], # [POODLE] U+E4DF -> U+E6A1
  ["eeb39c", :undef], # [DOLPHIN] U+EB1B -> "[イルカ]"
  ["ef83b2", :undef], # [MOUSE FACE] U+E5C2 -> "[ネズミ]"
  ["ef83b0", :undef], # [TIGER FACE] U+E5C0 -> "[トラ]"
  ["eebeb4", "ee9aa2"], # [CAT FACE] U+E4DB -> U+E6A2
  ["eebd88", :undef], # [SPOUTING WHALE] U+E470 -> "[クジラ]"
  ["eebeb1", "ee9d94"], # [HORSE FACE] U+E4D8 -> U+E754
  ["eebeb2", :undef], # [MONKEY FACE] U+E4D9 -> "[サル]"
  ["eebeba", "ee9aa1"], # [DOG FACE] U+E4E1 -> U+E6A1
  ["eebeb7", "ee9d95"], # [PIG FACE] U+E4DE -> U+E755
  ["ef83b1", :undef], # [BEAR FACE] U+E5C1 -> "[クマ]"
  ["eeb3a2", :undef], # [COW FACE] U+EB21 -> "[牛]"
  ["eebeb0", :undef], # [RABBIT FACE] U+E4D7 -> "[ウサギ]"
  ["eebeb3", :undef], # [FROG FACE] U+E4DA -> "[カエル]"
  ["eebf87", "ee9a98"], # [PAW PRINTS] U+E4EE -> U+E698
  ["eeb583", :undef], # [DRAGON FACE] U+EB3F -> "[辰]"
  ["eeb58a", :undef], # [PANDA FACE] U+EB46 -> "[パンダ]"
  ["eeb58c", "ee9d95"], # [PIG NOSE] U+EB48 -> U+E755
  ["eebd8a", "ee9bb1"], # [ANGRY FACE] U+E472 -> U+E6F1
  ["eeb5ab", "ee9bb3"], # [ANGUISHED FACE] U+EB67 -> U+E6F3
  ["eeb29e", "ee9bb4"], # [ASTONISHED FACE] U+EACA -> U+E6F4
  ["ef838b", "ee9bb4"], # [DIZZY FACE] U+E5AE -> U+E6F4
  ["eeb29f", "ee9ca3"], # [EXASPERATED FACE] U+EACB -> U+E723
  ["eeb29d", "ee9ca5"], # [EXPRESSIONLESS FACE] U+EAC9 -> U+E725
  ["ef83b4", "ee9ca6"], # [FACE WITH HEART-SHAPED EYES] U+E5C4 -> U+E726
  ["eeb295", "ee9d93"], # [FACE WITH LOOK OF TRIUMPH] U+EAC1 -> U+E753
  ["eebf80", "ee9ca8"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E4E7 -> U+E728
  ["eeb2a3", "ee9ca6"], # [FACE THROWING A KISS] U+EACF -> U+E726
  ["eeb2a2", "ee9ca6"], # [FACE KISSING] U+EACE -> U+E726
  ["eeb29b", :undef], # [FACE WITH MASK] U+EAC7 -> "[風邪ひき]"
  ["eeb29c", "ee9caa"], # [FLUSHED FACE] U+EAC8 -> U+E72A
  ["eebd89", "ee9bb0"], # [HAPPY FACE WITH OPEN MOUTH] U+E471 -> U+E6F0
  ["eeb685", "ee9d93"], # [HAPPY FACE WITH GRIN] U+EB80 -> U+E753
  ["eeb5a8", "ee9caa"], # [HAPPY AND CRYING FACE] U+EB64 -> U+E72A
  ["eeb2a1", "ee9bb0"], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+EACD -> U+E6F0
  ["eebf94", "ee9bb0"], # [WHITE SMILING FACE] U+E4FB -> U+E6F0
  ["eeb5ad", "ee9cae"], # [CRYING FACE] U+EB69 -> U+E72E
  ["eebd8b", "ee9cad"], # [LOUDLY CRYING FACE] U+E473 -> U+E72D
  ["eeb29a", "ee9d97"], # [FEARFUL FACE] U+EAC6 -> U+E757
  ["eeb296", "ee9cab"], # [PERSEVERING FACE] U+EAC2 -> U+E72B
  ["eeb5a1", "ee9ca4"], # [POUTING FACE] U+EB5D -> U+E724
  ["eeb299", "ee9ca1"], # [RELIEVED FACE] U+EAC5 -> U+E721
  ["eeb297", "ee9bb3"], # [CONFOUNDED FACE] U+EAC3 -> U+E6F3
  ["eeb294", "ee9ca0"], # [PENSIVE FACE] U+EAC0 -> U+E720
  ["ef83b5", "ee9d97"], # [FACE SCREAMING IN FEAR] U+E5C5 -> U+E757
  ["eeb298", "ee9c81"], # [SLEEPY FACE] U+EAC4 -> U+E701
  ["eeb293", "ee9cac"], # [SMIRKING FACE] U+EABF -> U+E72C
  ["ef83b6", "ee9ca3"], # [FACE WITH COLD SWEAT] U+E5C6 -> U+E723
  ["eebd8c", "ee9cab"], # [TIRED FACE] U+E474 -> U+E72B
  ["ef83b3", "ee9ca9"], # [WINKING FACE] U+E5C3 -> U+E729
  ["eeb5a5", "ee9bb0"], # [CAT FACE WITH OPEN MOUTH] U+EB61 -> U+E6F0
  ["eeb684", "ee9d93"], # [HAPPY CAT FACE WITH GRIN] U+EB7F -> U+E753
  ["eeb5a7", "ee9caa"], # [HAPPY AND CRYING CAT FACE] U+EB63 -> U+E72A
  ["eeb5a4", "ee9ca6"], # [CAT FACE KISSING] U+EB60 -> U+E726
  ["eeb5a9", "ee9ca6"], # [CAT FACE WITH HEART-SHAPED EYES] U+EB65 -> U+E726
  ["eeb5ac", "ee9cae"], # [CRYING CAT FACE] U+EB68 -> U+E72E
  ["eeb5a2", "ee9ca4"], # [POUTING CAT FACE] U+EB5E -> U+E724
  ["eeb5ae", "ee9d93"], # [CAT FACE WITH TIGHTLY-CLOSED LIPS] U+EB6A -> U+E753
  ["eeb5aa", "ee9bb3"], # [ANGUISHED CAT FACE] U+EB66 -> U+E6F3
  ["eeb2ab", "ee9caf"], # [FACE WITH NO GOOD GESTURE] U+EAD7 -> U+E72F
  ["eeb2ac", "ee9c8b"], # [FACE WITH OK GESTURE] U+EAD8 -> U+E70B
  ["eeb2ad", :undef], # [PERSON BOWING DEEPLY] U+EAD9 -> "m(_ _)m"
  ["eeb594", :undef], # [SEE-NO-EVIL MONKEY] U+EB50 -> "(/_＼)"
  ["eeb595", :undef], # [SPEAK-NO-EVIL MONKEY] U+EB51 -> "(・×・)"
  ["eeb596", :undef], # [HEAR-NO-EVIL MONKEY] U+EB52 -> "|(・×・)|"
  ["eeb68a", :undef], # [PERSON RAISING ONE HAND] U+EB85 -> "(^-^)/"
  ["eeb68b", :undef], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+EB86 -> "＼(^o^)／"
  ["eeb68c", "ee9bb3"], # [PERSON FROWNING] U+EB87 -> U+E6F3
  ["eeb68d", "ee9bb1"], # [PERSON WITH POUTING FACE] U+EB88 -> U+E6F1
  ["eeb2a6", :undef], # [PERSON WITH FOLDED HANDS] U+EAD2 -> "(&gt;人&lt;)"
  ["eebe84", "ee99a3"], # [HOUSE BUILDING] U+E4AB -> U+E663
  ["ef83a0", "ee99a3"], # [HOUSE WITH GARDEN] U+EB09 -> U+E663
  ["eebe86", "ee99a4"], # [OFFICE BUILDING] U+E4AD -> U+E664
  ["eeb191", "ee99a5"], # [JAPANESE POST OFFICE] U+E5DE -> U+E665
  ["eeb192", "ee99a6"], # [HOSPITAL] U+E5DF -> U+E666
  ["eebe83", "ee99a7"], # [BANK] U+E4AA -> U+E667
  ["eebdbb", "ee99a8"], # [AUTOMATED TELLER MACHINE] U+E4A3 -> U+E668
  ["eeb194", "ee99a9"], # [HOTEL] U+EA81 -> U+E669
  ["eeb387", "ee99a9ee9baf"], # [LOVE HOTEL] U+EAF3 -> U+E669 U+E6EF
  ["eebdbc", "ee99aa"], # [CONVENIENCE STORE] U+E4A4 -> U+E66A
  ["eeb193", "ee9cbe"], # [SCHOOL] U+EA80 -> U+E73E
  ["ef83ab", :undef], # [CHURCH] U+E5BB -> "[教会]"
  ["eeb182", :undef], # [FOUNTAIN] U+E5CF -> "[噴水]"
  ["eeb38a", :undef], # [DEPARTMENT STORE] U+EAF6 -> "[デパート]"
  ["eeb38b", :undef], # [JAPANESE CASTLE] U+EAF7 -> "[城]"
  ["eeb38c", :undef], # [EUROPEAN CASTLE] U+EAF8 -> "[城]"
  ["eeb38d", :undef], # [FACTORY] U+EAF9 -> "[工場]"
  ["eebe82", "ee99a1"], # [ANCHOR] U+E4A9 -> U+E661
  ["eebe96", "ee9d8b"], # [IZAKAYA LANTERN] U+E4BD -> U+E74B
  ["ef83ad", "ee9d80"], # [MOUNT FUJI] U+E5BD -> U+E740
  ["eebe99", :undef], # [TOKYO TOWER] U+E4C0 -> "[東京タワー]"
  ["ef828f", :undef], # [SILHOUETTE OF JAPAN] U+E572 -> "[日本地図]"
  ["eeb5b0", :undef], # [MOYAI] U+EB6C -> "[モアイ]"
  ["ef83a7", "ee9a99"], # [MANS SHOE] U+E5B7 -> U+E699
  ["eeb3ac", "ee9a99"], # [ATHLETIC SHOE] U+EB2B -> U+E699
  ["eebfb3", "ee99b4"], # [HIGH-HEELED SHOE] U+E51A -> U+E674
  ["eeb1b2", :undef], # [WOMANS BOOTS] U+EA9F -> "[ブーツ]"
  ["eeb3ab", "ee9a98"], # [FOOTPRINTS] U+EB2A -> U+E698
  ["eebf97", "ee9a9a"], # [EYEGLASSES] U+E4FE -> U+E69A
  ["ef83a6", "ee9c8e"], # [T-SHIRT] U+E5B6 -> U+E70E
  ["eeb5bb", "ee9c91"], # [JEANS] U+EB77 -> U+E711
  ["ef83b9", "ee9c9a"], # [CROWN] U+E5C9 -> U+E71A
  ["eeb1a6", :undef], # [NECKTIE] U+EA93 -> "[ネクタイ]"
  ["eeb1b1", :undef], # [WOMANS HAT] U+EA9E -> "[帽子]"
  ["eeb5af", :undef], # [DRESS] U+EB6B -> "[ドレス]"
  ["eeb1b6", :undef], # [KIMONO] U+EAA3 -> "[着物]"
  ["eeb1b7", :undef], # [BIKINI] U+EAA4 -> "[ビキニ]"
  ["eebfa6", "ee9c8e"], # [WOMANS CLOTHES] U+E50D -> U+E70E
  ["eebf9d", "ee9c8f"], # [PURSE] U+E504 -> U+E70F
  ["eebdb4", "ee9a82"], # [HANDBAG] U+E49C -> U+E682
  ["eebea0", "ee9c95"], # [MONEY BAG] U+E4C7 -> U+E715
  ["eeb18f", :undef], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+E5DC -> "[株価]"
  ["ef8296", "ee9c95"], # [HEAVY DOLLAR SIGN] U+E579 -> U+E715
  ["ef8299", :undef], # [CREDIT CARD] U+E57C -> "[カード]"
  ["ef829a", "ee9b96"], # [BANKNOTE WITH YEN SIGN] U+E57D -> U+E6D6
  ["ef82a2", "ee9c95"], # [BANKNOTE WITH DOLLAR SIGN] U+E585 -> U+E715
  ["eeb59f", :undef], # [MONEY WITH WINGS] U+EB5B -> "[飛んでいくお金]"
  ["eeb392", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS CN] U+EB11 -> "[中国]"
  ["eeb38f", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS DE] U+EB0E -> "[ドイツ]"
  ["eeb188", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS ES] U+E5D5 -> "[スペイン]"
  ["eeb38e", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS FR] U+EAFA -> "[フランス]"
  ["eeb391", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS GB] U+EB10 -> "[イギリス]"
  ["eeb390", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS IT] U+EB0F -> "[イタリア]"
  ["eebea5", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS JP] U+E4CC -> "[日の丸]"
  ["eeb393", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS KR] U+EB12 -> "[韓国]"
  ["eeb189", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS RU] U+E5D6 -> "[ロシア]"
  ["ef8290", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS US] U+E573 -> "[USA]"
  ["eebd93", :undef], # [FIRE] U+E47B -> "[炎]"
  ["ef82a0", "ee9bbb"], # [ELECTRIC TORCH] U+E583 -> U+E6FB
  ["ef82a4", "ee9c98"], # [WRENCH] U+E587 -> U+E718
  ["ef83bb", :undef], # [HAMMER] U+E5CB -> "[ハンマー]"
  ["ef829e", :undef], # [NUT AND BOLT] U+E581 -> "[ネジ]"
  ["ef829c", :undef], # [HOCHO] U+E57F -> "[包丁]"
  ["eebfa3", :undef], # [PISTOL] U+E50A -> "[ピストル]"
  ["eeb1a2", :undef], # [CRYSTAL BALL] U+EA8F -> "[占い]"
  ["eebd98", :undef], # [JAPANESE SYMBOL FOR BEGINNER] U+E480 -> "[若葉マーク]"
  ["eebfa9", :undef], # [SYRINGE] U+E510 -> "[注射]"
  ["eeb1ad", :undef], # [PILL] U+EA9A -> "[薬]"
  ["eeb3a7", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+EB26 -> "[A]"
  ["eeb3a8", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+EB27 -> "[B]"
  ["eeb3aa", :undef], # [NEGATIVE SQUARED AB] U+EB29 -> "[AB]"
  ["eeb3a9", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+EB28 -> "[O]"
  ["ef82bc", "ee9a84"], # [RIBBON] U+E59F -> U+E684
  ["eebea8", "ee9a85"], # [WRAPPED PRESENT] U+E4CF -> U+E685
  ["ef82bd", "ee9a86"], # [BIRTHDAY CAKE] U+E5A0 -> U+E686
  ["eebea2", "ee9aa4"], # [CHRISTMAS TREE] U+E4C9 -> U+E6A4
  ["eeb384", :undef], # [FATHER CHRISTMAS] U+EAF0 -> "[サンタ]"
  ["eeb18c", :undef], # [CROSSED FLAGS] U+E5D9 -> "[祝日]"
  ["ef83bc", :undef], # [FIREWORKS] U+E5CC -> "[花火]"
  ["eeb1ae", :undef], # [BALLOON] U+EA9B -> "[風船]"
  ["eeb1af", :undef], # [PARTY POPPER] U+EA9C -> "[クラッカー]"
  ["eeb2b7", :undef], # [PINE DECORATION] U+EAE3 -> "[門松]"
  ["eeb2b8", :undef], # [JAPANESE DOLLS] U+EAE4 -> "[ひな祭り]"
  ["eeb2b9", :undef], # [GRADUATION CAP] U+EAE5 -> "[卒業式]"
  ["eeb2ba", :undef], # [SCHOOL SATCHEL] U+EAE6 -> "[ランドセル]"
  ["eeb2bb", :undef], # [CARP STREAMER] U+EAE7 -> "[こいのぼり]"
  ["eeb2bf", :undef], # [FIREWORK SPARKLER] U+EAEB -> "[線香花火]"
  ["eeb381", :undef], # [WIND CHIME] U+EAED -> "[風鈴]"
  ["eeb382", :undef], # [JACK-O-LANTERN] U+EAEE -> "[ハロウィン]"
  ["eebd87", :undef], # [CONFETTI BALL] U+E46F -> "[オメデトウ]"
  ["eeb581", :undef], # [TANABATA TREE] U+EB3D -> "[七夕]"
  ["eeb383", :undef], # [MOON VIEWING CEREMONY] U+EAEF -> "[お月見]"
  ["ef82b8", "ee999a"], # [PAGER] U+E59B -> U+E65A
  ["ef82b3", "ee9a87"], # [BLACK TELEPHONE] U+E596 -> U+E687
  ["eebfb7", "ee9a87"], # [TELEPHONE RECEIVER] U+E51E -> U+E687
  ["ef82a5", "ee9a88"], # [MOBILE PHONE] U+E588 -> U+E688
  ["ef839f", "ee9b8e"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+EB08 -> U+E6CE
  ["eeb1a5", "ee9a89"], # [MEMO] U+EA92 -> U+E689
  ["eebfb9", "ee9b90"], # [FAX MACHINE] U+E520 -> U+E6D0
  ["eebfba", "ee9b93"], # [ENVELOPE] U+E521 -> U+E6D3
  ["ef82ae", "ee9b8f"], # [INCOMING ENVELOPE] U+E591 -> U+E6CF
  ["eeb5a6", "ee9b8f"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+EB62 -> U+E6CF
  ["eebfb4", "ee99a5"], # [CLOSED MAILBOX WITH LOWERED FLAG] U+E51B -> U+E665
  ["ef83a1", "ee99a5"], # [CLOSED MAILBOX WITH RAISED FLAG] U+EB0A -> U+E665
  ["ef82a8", :undef], # [NEWSPAPER] U+E58B -> "[新聞]"
  ["eebe81", :undef], # [SATELLITE ANTENNA] U+E4A8 -> "[アンテナ]"
  ["ef82af", :undef], # [OUTBOX TRAY] U+E592 -> "[送信BOX]"
  ["ef82b0", :undef], # [INBOX TRAY] U+E593 -> "[受信BOX]"
  ["eebfb8", "ee9a85"], # [PACKAGE] U+E51F -> U+E685
  ["eeb5b5", "ee9b93"], # [E-MAIL SYMBOL] U+EB71 -> U+E6D3
  ["ef8394", :undef], # [INPUT SYMBOL FOR LATIN CAPITAL LETTERS] U+EAFD -> "[ABCD]"
  ["ef8395", :undef], # [INPUT SYMBOL FOR LATIN SMALL LETTERS] U+EAFE -> "[abcd]"
  ["ef8396", :undef], # [INPUT SYMBOL FOR NUMBERS] U+EAFF -> "[1234]"
  ["ef8397", :undef], # [INPUT SYMBOL FOR SYMBOLS] U+EB00 -> "[記号]"
  ["eeb599", :undef], # [INPUT SYMBOL FOR LATIN LETTERS] U+EB55 -> "[ABC]"
  ["ef839a", "ee9aae"], # [BLACK NIB] U+EB03 -> U+E6AE
  ["ef83a8", "ee9c96"], # [PERSONAL COMPUTER] U+E5B8 -> U+E716
  ["eebdb9", "ee9c99"], # [PENCIL] U+E4A1 -> U+E719
  ["eebdb8", "ee9cb0"], # [PAPERCLIP] U+E4A0 -> U+E730
  ["eeb181", "ee9a82"], # [BRIEFCASE] U+E5CE -> U+E682
  ["ef829f", :undef], # [MINIDISC] U+E582 -> "[MD]"
  ["ef81be", :undef], # [FLOPPY DISK] U+E562 -> "[フロッピー]"
  ["eebfa5", "ee9a8c"], # [OPTICAL DISC] U+E50C -> U+E68C
  ["eebfaf", "ee99b5"], # [BLACK SCISSORS] U+E516 -> U+E675
  ["ef81bc", :undef], # [ROUND PUSHPIN] U+E560 -> "[画びょう]"
  ["ef81bd", "ee9a89"], # [PAGE WITH CURL] U+E561 -> U+E689
  ["ef8286", "ee9a89"], # [PAGE FACING UP] U+E569 -> U+E689
  ["ef8280", :undef], # [CALENDAR] U+E563 -> "[カレンダー]"
  ["ef82ac", :undef], # [FILE FOLDER] U+E58F -> "[フォルダ]"
  ["ef82ad", :undef], # [OPEN FILE FOLDER] U+E590 -> "[フォルダ]"
  ["ef8288", "ee9a83"], # [NOTEBOOK] U+E56B -> U+E683
  ["eebdb7", "ee9a83"], # [OPEN BOOK] U+E49F -> U+E683
  ["eebdb5", "ee9a83"], # [NOTEBOOK WITH DECORATIVE COVER] U+E49D -> U+E683
  ["ef8285", "ee9a83"], # [CLOSED BOOK] U+E568 -> U+E683
  ["ef8282", "ee9a83"], # [GREEN BOOK] U+E565 -> U+E683
  ["ef8283", "ee9a83"], # [BLUE BOOK] U+E566 -> U+E683
  ["ef8284", "ee9a83"], # [ORANGE BOOK] U+E567 -> U+E683
  ["ef828c", "ee9a83"], # [BOOKS] U+E56F -> U+E683
  ["eebfb6", :undef], # [NAME BADGE] U+E51D -> "[名札]"
  ["ef81bb", "ee9c8a"], # [SCROLL] U+E55F -> U+E70A
  ["ef8281", "ee9a89"], # [CLIPBOARD] U+E564 -> U+E689
  ["ef8287", :undef], # [TEAR-OFF CALENDAR] U+E56A -> "[カレンダー]"
  ["ef8291", :undef], # [BAR CHART] U+E574 -> "[グラフ]"
  ["ef8292", :undef], # [CHART WITH UPWARDS TREND] U+E575 -> "[グラフ]"
  ["ef8293", :undef], # [CHART WITH DOWNWARDS TREND] U+E576 -> "[グラフ]"
  ["ef8289", "ee9a83"], # [CARD INDEX] U+E56C -> U+E683
  ["ef828a", :undef], # [PUSHPIN] U+E56D -> "[画びょう]"
  ["ef828b", "ee9a83"], # [LEDGER] U+E56E -> U+E683
  ["ef828d", :undef], # [STRAIGHT RULER] U+E570 -> "[定規]"
  ["eebdba", :undef], # [TRIANGULAR RULER] U+E4A2 -> "[三角定規]"
  ["ef83a2", "ee9a89"], # [BOOKMARK TABS] U+EB0B -> U+E689
  ["eebe93", "ee9993"], # [BASEBALL] U+E4BA -> U+E653
  ["ef82b6", "ee9994"], # [FLAG IN HOLE] U+E599 -> U+E654
  ["eebe90", "ee9995"], # [TENNIS RACQUET AND BALL] U+E4B7 -> U+E655
  ["eebe8f", "ee9996"], # [SOCCER BALL] U+E4B6 -> U+E656
  ["eeb280", "ee9997"], # [SKI AND SKI BOOT] U+EAAC -> U+E657
  ["ef82b7", "ee9998"], # [BASKETBALL AND HOOP] U+E59A -> U+E658
  ["eebe92", "ee9999"], # [CHEQUERED FLAG] U+E4B9 -> U+E659
  ["eebe91", "ee9c92"], # [SNOWBOARDER] U+E4B8 -> U+E712
  ["eebd83", "ee9cb3"], # [RUNNER] U+E46B -> U+E733
  ["eeb585", "ee9c92"], # [SURFER] U+EB41 -> U+E712
  ["eeb186", :undef], # [TROPHY] U+E5D3 -> "[トロフィー]"
  ["eebe94", :undef], # [AMERICAN FOOTBALL] U+E4BB -> "[フットボール]"
  ["eeb2b2", :undef], # [SWIMMER] U+EADE -> "[水泳]"
  ["eebe8e", "ee999b"], # [TRAIN] U+E4B5 -> U+E65B
  ["ef83ac", "ee999c"], # [METRO] U+E5BC -> U+E65C
  ["eebe89", "ee999d"], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+E4B0 -> U+E65D
  ["eebe8a", "ee999e"], # [AUTOMOBILE] U+E4B1 -> U+E65E
  ["eebe88", "ee99a0"], # [ONCOMING BUS] U+E4AF -> U+E660
  ["eebe80", :undef], # [BUS STOP] U+E4A7 -> "[バス停]"
  ["eeb195", "ee99a1"], # [SHIP] U+EA82 -> U+E661
  ["eebe8c", "ee99a2"], # [AIRPLANE] U+E4B3 -> U+E662
  ["eebe8d", "ee9aa3"], # [SAILBOAT] U+E4B4 -> U+E6A3
  ["eeb5b1", :undef], # [STATION] U+EB6D -> "[駅]"
  ["ef83b8", :undef], # [ROCKET] U+E5C8 -> "[ロケット]"
  ["eebe8b", :undef], # [DELIVERY TRUCK] U+E4B2 -> "[トラック]"
  ["eeb2b3", :undef], # [FIRE ENGINE] U+EADF -> "[消防車]"
  ["eeb2b4", :undef], # [AMBULANCE] U+EAE0 -> "[救急車]"
  ["eeb2b5", :undef], # [POLICE CAR] U+EAE1 -> "[パトカー]"
  ["ef828e", "ee99ab"], # [FUEL PUMP] U+E571 -> U+E66B
  ["eebdbe", "ee99ac"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E4A6 -> U+E66C
  ["eebd82", "ee99ad"], # [HORIZONTAL TRAFFIC LIGHT] U+E46A -> U+E66D
  ["eeb18a", :undef], # [CONSTRUCTION SIGN] U+E5D7 -> "[工事中]"
  ["eeb5b7", :undef], # [POLICE CARS REVOLVING LIGHT] U+EB73 -> "[パトカー]"
  ["eebe95", "ee9bb7"], # [HOT SPRINGS] U+E4BC -> U+E6F7
  ["eeb183", :undef], # [TENT] U+E5D0 -> "[キャンプ]"
  ["eebd85", :undef], # [FERRIS WHEEL] U+E46D -> "[観覧車]"
  ["eeb2b6", :undef], # [ROLLER COASTER] U+EAE2 -> "[ジェットコースター]"
  ["eeb586", "ee9d91"], # [FISHING POLE AND FISH] U+EB42 -> U+E751
  ["eebf9c", "ee99b6"], # [MICROPHONE] U+E503 -> U+E676
  ["eebfb0", "ee99b7"], # [MOVIE CAMERA] U+E517 -> U+E677
  ["eebfa1", "ee99ba"], # [HEADPHONE] U+E508 -> U+E67A
  ["ef82b9", "ee99bb"], # [ARTIST PALETTE] U+E59C -> U+E67B
  ["eeb389", "ee99bc"], # [TOP HAT] U+EAF5 -> U+E67C
  ["ef82bb", "ee99bd"], # [CIRCUS TENT] U+E59E -> U+E67D
  ["eebdb6", "ee99be"], # [TICKET] U+E49E -> U+E67E
  ["eebe97", "ee9aac"], # [CLAPPER BOARD] U+E4BE -> U+E6AC
  ["ef82ba", :undef], # [PERFORMING ARTS] U+E59D -> "[演劇]"
  ["eebe9f", "ee9a8b"], # [VIDEO GAME] U+E4C6 -> U+E68B
  ["eeb184", :undef], # [MAHJONG TILE RED DRAGON] U+E5D1 -> "[麻雀]"
  ["eebe9e", :undef], # [DIRECT HIT] U+E4C5 -> "[的中]"
  ["eebd86", :undef], # [SLOT MACHINE] U+E46E -> "[777]"
  ["eeb2b1", :undef], # [BILLIARDS] U+EADD -> "[ビリヤード]"
  ["eebea1", :undef], # [GAME DIE] U+E4C8 -> "[サイコロ]"
  ["eeb587", :undef], # [BOWLING] U+EB43 -> "[ボーリング]"
  ["eeb5b2", :undef], # [FLOWER PLAYING CARDS] U+EB6E -> "[花札]"
  ["eeb5b3", :undef], # [PLAYING CARD BLACK JOKER] U+EB6F -> "[ジョーカー]"
  ["ef83ae", "ee9bb6"], # [MUSICAL NOTE] U+E5BE -> U+E6F6
  ["eebf9e", "ee9bbf"], # [MULTIPLE MUSICAL NOTES] U+E505 -> U+E6FF
  ["eebf9f", :undef], # [GUITAR] U+E506 -> "[ギター]"
  ["eeb584", :undef], # [MUSICAL KEYBOARD] U+EB40 -> "[ピアノ]"
  ["eeb2b0", :undef], # [TRUMPET] U+EADC -> "[トランペット]"
  ["eebfa0", :undef], # [VIOLIN] U+E507 -> "[バイオリン]"
  ["eeb2a0", "ee9bbf"], # [MUSICAL SCORE] U+EACC -> U+E6FF
  ["eebfae", "ee9a81"], # [CAMERA] U+E515 -> U+E681
  ["ef829b", "ee99b7"], # [VIDEO CAMERA] U+E57E -> U+E677
  ["eebf9b", "ee9a8a"], # [TELEVISION] U+E502 -> U+E68A
  ["ef83a9", :undef], # [RADIO] U+E5B9 -> "[ラジオ]"
  ["ef829d", :undef], # [VIDEOCASSETTE] U+E580 -> "[ビデオ]"
  ["eebf84", "ee9bb9"], # [KISS MARK] U+E4EB -> U+E6F9
  ["eeb5bc", "ee9c97"], # [LOVE LETTER] U+EB78 -> U+E717
  ["eebfad", "ee9c9b"], # [RING] U+E514 -> U+E71B
  ["ef83ba", "ee9bb9"], # [KISS] U+E5CA -> U+E6F9
  ["eeb1a8", :undef], # [BOUQUET] U+EA95 -> "[花束]"
  ["eeb2ae", "ee9bad"], # [COUPLE WITH HEART] U+EADA -> U+E6ED
  ["eeb196", :undef], # [NO ONE UNDER EIGHTEEN SYMBOL] U+EA83 -> "[18禁]"
  ["ef81b4", "ee9cb1"], # [COPYRIGHT SIGN] U+E558 -> U+E731
  ["ef81b5", "ee9cb6"], # [REGISTERED SIGN] U+E559 -> U+E736
  ["ef81aa", "ee9cb2"], # [TRADE MARK SIGN] U+E54E -> U+E732
  ["ef818f", :undef], # [INFORMATION SOURCE] U+E533 -> "[ｉ]"
  ["eeb689", "ee9ba0"], # [HASH KEY] U+EB84 -> U+E6E0
  ["eebfbb", "ee9ba2"], # [KEYCAP 1] U+E522 -> U+E6E2
  ["eebfbc", "ee9ba3"], # [KEYCAP 2] U+E523 -> U+E6E3
  ["ef8180", "ee9ba4"], # [KEYCAP 3] U+E524 -> U+E6E4
  ["ef8181", "ee9ba5"], # [KEYCAP 4] U+E525 -> U+E6E5
  ["ef8182", "ee9ba6"], # [KEYCAP 5] U+E526 -> U+E6E6
  ["ef8183", "ee9ba7"], # [KEYCAP 6] U+E527 -> U+E6E7
  ["ef8184", "ee9ba8"], # [KEYCAP 7] U+E528 -> U+E6E8
  ["ef8185", "ee9ba9"], # [KEYCAP 8] U+E529 -> U+E6E9
  ["ef8186", "ee9baa"], # [KEYCAP 9] U+E52A -> U+E6EA
  ["ef8389", "ee9bab"], # [KEYCAP 0] U+E5AC -> U+E6EB
  ["ef8187", :undef], # [KEYCAP TEN] U+E52B -> "[10]"
  ["eeb197", :undef], # [ANTENNA WITH BARS] U+EA84 -> "[バリ3]"
  ["eeb1a3", :undef], # [VIBRATION MODE] U+EA90 -> "[マナーモード]"
  ["eeb1a4", :undef], # [MOBILE PHONE OFF] U+EA91 -> "[ケータイOFF]"
  ["eebeaf", "ee99b3"], # [HAMBURGER] U+E4D6 -> U+E673
  ["eebeae", "ee9d89"], # [RICE BALL] U+E4D5 -> U+E749
  ["eebea9", "ee9d8a"], # [SHORTCAKE] U+E4D0 -> U+E74A
  ["ef8391", "ee9d8c"], # [STEAMING BOWL] U+E5B4 -> U+E74C
  ["eeb283", "ee9d8d"], # [BREAD] U+EAAF -> U+E74D
  ["eebeaa", :undef], # [COOKING] U+E4D1 -> "[フライパン]"
  ["eeb284", :undef], # [SOFT ICE CREAM] U+EAB0 -> "[ソフトクリーム]"
  ["eeb285", :undef], # [FRENCH FRIES] U+EAB1 -> "[ポテト]"
  ["eeb286", :undef], # [DANGO] U+EAB2 -> "[だんご]"
  ["eeb287", :undef], # [RICE CRACKER] U+EAB3 -> "[せんべい]"
  ["eeb288", "ee9d8c"], # [COOKED RICE] U+EAB4 -> U+E74C
  ["eeb289", :undef], # [SPAGHETTI] U+EAB5 -> "[パスタ]"
  ["eeb28a", :undef], # [CURRY AND RICE] U+EAB6 -> "[カレー]"
  ["eeb28b", :undef], # [ODEN] U+EAB7 -> "[おでん]"
  ["eeb28c", :undef], # [SUSHI] U+EAB8 -> "[すし]"
  ["eeb291", :undef], # [BENTO BOX] U+EABD -> "[弁当]"
  ["eeb292", :undef], # [POT OF FOOD] U+EABE -> "[鍋]"
  ["eeb2be", :undef], # [SHAVED ICE] U+EAEA -> "[カキ氷]"
  ["eebe9d", :undef], # [MEAT ON BONE] U+E4C4 -> "[肉]"
  ["eebf86", "ee9983"], # [FISH CAKE WITH SWIRL DESIGN] U+E4ED -> U+E643
  ["eeb3bb", :undef], # [ROASTED SWEET POTATO] U+EB3A -> "[やきいも]"
  ["eeb3bc", :undef], # [SLICE OF PIZZA] U+EB3B -> "[ピザ]"
  ["eeb580", :undef], # [POULTRY LEG] U+EB3C -> "[チキン]"
  ["eeb58e", :undef], # [ICE CREAM] U+EB4A -> "[アイスクリーム]"
  ["eeb58f", :undef], # [DOUGHNUT] U+EB4B -> "[ドーナツ]"
  ["eeb590", :undef], # [COOKIE] U+EB4C -> "[クッキー]"
  ["eeb591", :undef], # [CHOCOLATE BAR] U+EB4D -> "[チョコ]"
  ["eeb592", :undef], # [CANDY] U+EB4E -> "[キャンディ]"
  ["eeb593", :undef], # [LOLLIPOP] U+EB4F -> "[キャンディ]"
  ["eeb59a", :undef], # [CUSTARD] U+EB56 -> "[プリン]"
  ["eeb59d", :undef], # [HONEY POT] U+EB59 -> "[ハチミツ]"
  ["eeb5b4", :undef], # [FRIED SHRIMP] U+EB70 -> "[エビフライ]"
  ["eebe85", "ee99af"], # [FORK AND KNIFE] U+E4AC -> U+E66F
  ["ef82b4", "ee99b0"], # [HOT BEVERAGE] U+E597 -> U+E670
  ["eebe9b", "ee99b1"], # [COCKTAIL GLASS] U+E4C2 -> U+E671
  ["eebe9c", "ee99b2"], # [BEER MUG] U+E4C3 -> U+E672
  ["eeb282", "ee9c9e"], # [TEACUP WITHOUT HANDLE] U+EAAE -> U+E71E
  ["eeb1aa", "ee9d8b"], # [SAKE BOTTLE AND CUP] U+EA97 -> U+E74B
  ["eebe9a", "ee9d96"], # [WINE GLASS] U+E4C1 -> U+E756
  ["eeb1ab", "ee99b2"], # [CLINKING BEER MUGS] U+EA98 -> U+E672
  ["eeb582", "ee99b1"], # [TROPICAL DRINK] U+EB3E -> U+E671
  ["ef81b1", "ee99b8"], # [NORTH EAST ARROW] U+E555 -> U+E678
  ["ef81a9", "ee9a96"], # [SOUTH EAST ARROW] U+E54D -> U+E696
  ["ef81a8", "ee9a97"], # [NORTH WEST ARROW] U+E54C -> U+E697
  ["ef81b2", "ee9aa5"], # [SOUTH WEST ARROW] U+E556 -> U+E6A5
  ["eeb3ae", "ee9bb5"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+EB2D -> U+E6F5
  ["eeb3af", "ee9c80"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+EB2E -> U+E700
  ["eeb5be", "ee9cbc"], # [LEFT RIGHT ARROW] U+EB7A -> U+E73C
  ["eeb680", "ee9cbd"], # [UP DOWN ARROW] U+EB7B -> U+E73D
  ["ef819b", :undef], # [UPWARDS BLACK ARROW] U+E53F -> "[↑]"
  ["ef819c", :undef], # [DOWNWARDS BLACK ARROW] U+E540 -> "[↓]"
  ["ef81ae", :undef], # [BLACK RIGHTWARDS ARROW] U+E552 -> "[→]"
  ["ef81af", :undef], # [LEFTWARDS BLACK ARROW] U+E553 -> "[←]"
  ["ef818a", :undef], # [BLACK RIGHT-POINTING TRIANGLE] U+E52E -> "[&gt;]"
  ["ef8189", :undef], # [BLACK LEFT-POINTING TRIANGLE] U+E52D -> "[&lt;]"
  ["ef818c", :undef], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+E530 -> "[&gt;&gt;]"
  ["ef818b", :undef], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+E52F -> "[&lt;&lt;]"
  ["ef81a1", :undef], # [BLACK UP-POINTING DOUBLE TRIANGLE] U+E545 -> "▲"
  ["ef81a0", :undef], # [BLACK DOWN-POINTING DOUBLE TRIANGLE] U+E544 -> "▼"
  ["ef81b6", :undef], # [UP-POINTING RED TRIANGLE] U+E55A -> "▲"
  ["ef81b7", :undef], # [DOWN-POINTING RED TRIANGLE] U+E55B -> "▼"
  ["ef819f", :undef], # [UP-POINTING SMALL RED TRIANGLE] U+E543 -> "▲"
  ["ef819e", :undef], # [DOWN-POINTING SMALL RED TRIANGLE] U+E542 -> "▼"
  ["eeb281", "ee9aa0"], # [HEAVY LARGE CIRCLE] U+EAAD -> U+E6A0
  ["ef81ac", :undef], # [CROSS MARK] U+E550 -> "[×]"
  ["ef81ad", :undef], # [NEGATIVE SQUARED CROSS MARK] U+E551 -> "[×]"
  ["eebd9a", "ee9c82"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E482 -> U+E702
  ["eeb3b0", "ee9c83"], # [EXCLAMATION QUESTION MARK] U+EB2F -> U+E703
  ["eeb3b1", "ee9c84"], # [DOUBLE EXCLAMATION MARK] U+EB30 -> U+E704
  ["eebd9b", :undef], # [BLACK QUESTION MARK ORNAMENT] U+E483 -> "[？]"
  ["eeb3b2", "ee9c8a"], # [CURLY LOOP] U+EB31 -> U+E70A
  ["ef82b2", "ee9bac"], # [HEAVY BLACK HEART] U+E595 -> U+E6EC
  ["eeb5b9", "ee9bad"], # [BEATING HEART] U+EB75 -> U+E6ED
  ["eebd8f", "ee9bae"], # [BROKEN HEART] U+E477 -> U+E6EE
  ["eebd90", "ee9baf"], # [TWO HEARTS] U+E478 -> U+E6EF
  ["eeb1b9", "ee9bac"], # [SPARKLING HEART] U+EAA6 -> U+E6EC
  ["eebf83", "ee9bac"], # [HEART WITH ARROW] U+E4EA -> U+E6EC
  ["eeb1ba", "ee9bac"], # [BLUE HEART] U+EAA7 -> U+E6EC
  ["eeb1bb", "ee9bac"], # [GREEN HEART] U+EAA8 -> U+E6EC
  ["eeb1bc", "ee9bac"], # [YELLOW HEART] U+EAA9 -> U+E6EC
  ["eeb1bd", "ee9bac"], # [PURPLE HEART] U+EAAA -> U+E6EC
  ["eeb598", "ee9bac"], # [HEART WITH RIBBON] U+EB54 -> U+E6EC
  ["ef838c", "ee9bad"], # [REVOLVING HEARTS] U+E5AF -> U+E6ED
  ["eeb1b8", "ee9a8d"], # [BLACK HEART SUIT] U+EAA5 -> U+E68D
  ["ef82be", "ee9a8e"], # [BLACK SPADE SUIT] U+E5A1 -> U+E68E
  ["ef82bf", "ee9a8f"], # [BLACK DIAMOND SUIT] U+E5A2 -> U+E68F
  ["ef8380", "ee9a90"], # [BLACK CLUB SUIT] U+E5A3 -> U+E690
  ["eebd95", "ee99bf"], # [SMOKING SYMBOL] U+E47D -> U+E67F
  ["eebd96", "ee9a80"], # [NO SMOKING SYMBOL] U+E47E -> U+E680
  ["eebd97", "ee9a9b"], # [WHEELCHAIR SYMBOL] U+E47F -> U+E69B
  ["eeb3ad", "ee9b9e"], # [TRIANGULAR FLAG ON POST] U+EB2C -> U+E6DE
  ["eebd99", "ee9cb7"], # [WARNING SIGN] U+E481 -> U+E737
  ["eebd9c", "ee9caf"], # [NO ENTRY] U+E484 -> U+E72F
  ["eeb5bd", "ee9cb5"], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+EB79 -> U+E735
  ["eebe87", "ee9c9d"], # [BICYCLE] U+E4AE -> U+E71D
  ["eeb5b6", "ee9cb3"], # [PEDESTRIAN] U+EB72 -> U+E733
  ["eeb18b", "ee9bb7"], # [BATH] U+E5D8 -> U+E6F7
  ["eebdbd", "ee99ae"], # [RESTROOM] U+E4A5 -> U+E66E
  ["ef819d", "ee9cb8"], # [NO ENTRY SIGN] U+E541 -> U+E738
  ["ef81b3", :undef], # [HEAVY CHECK MARK] U+E557 -> "[チェックマーク]"
  ["ef8388", "ee9b9b"], # [SQUARED CL] U+E5AB -> U+E6DB
  ["eeb198", :undef], # [SQUARED COOL] U+EA85 -> "[COOL]"
  ["ef8295", "ee9b97"], # [SQUARED FREE] U+E578 -> U+E6D7
  ["eeb19b", "ee9b98"], # [SQUARED ID] U+EA88 -> U+E6D8
  ["ef83a5", "ee9b9d"], # [SQUARED NEW] U+E5B5 -> U+E6DD
  ["ef838a", "ee9c8b"], # [SQUARED OK] U+E5AD -> U+E70B
  ["eebf81", :undef], # [SQUARED SOS] U+E4E8 -> "[SOS]"
  ["eebfa8", :undef], # [SQUARED UP WITH EXCLAMATION MARK] U+E50F -> "[UP!]"
  ["eeb185", :undef], # [SQUARED VS] U+E5D2 -> "[VS]"
  ["eeb19a", :undef], # [SQUARED KATAKANA SA] U+EA87 -> "[サービス]"
  ["eeb19d", "ee9cb9"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+EA8A -> U+E739
  ["eeb19c", "ee9cbb"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+EA89 -> U+E73B
  ["eeb199", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+EA86 -> "[割]"
  ["eeb19e", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+EA8B -> "[指]"
  ["eeb19f", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+EA8C -> "[営]"
  ["eebf8a", "ee9cb4"], # [CIRCLED IDEOGRAPH SECRET] U+E4F1 -> U+E734
  ["eeb1ac", :undef], # [CIRCLED IDEOGRAPH CONGRATULATION] U+EA99 -> "[祝]"
  ["eebf90", :undef], # [CIRCLED IDEOGRAPH ADVANTAGE] U+E4F7 -> "[得]"
  ["ef8398", :undef], # [CIRCLED IDEOGRAPH ACCEPT] U+EB01 -> "[可]"
  ["ef8198", :undef], # [HEAVY PLUS SIGN] U+E53C -> "[＋]"
  ["ef8199", :undef], # [HEAVY MINUS SIGN] U+E53D -> "[－]"
  ["ef81ab", :undef], # [HEAVY MULTIPLICATION X] U+E54F -> "[×]"
  ["ef81b0", :undef], # [HEAVY DIVISION SIGN] U+E554 -> "[÷]"
  ["eebd8e", "ee9bbb"], # [ELECTRIC LIGHT BULB] U+E476 -> U+E6FB
  ["eebebe", "ee9bbc"], # [ANGER SYMBOL] U+E4E5 -> U+E6FC
  ["eebd92", "ee9bbe"], # [BOMB] U+E47A -> U+E6FE
  ["eebd8d", "ee9c81"], # [SLEEPING SYMBOL] U+E475 -> U+E701
  ["ef838d", "ee9c85"], # [COLLISION SYMBOL] U+E5B0 -> U+E705
  ["ef838e", "ee9c86"], # [SPLASHING SWEAT SYMBOL] U+E5B1 -> U+E706
  ["eebebf", "ee9c87"], # [DROP OF WATER] U+E4E6 -> U+E707
  ["eebf8d", "ee9c88"], # [DASH SYMBOL] U+E4F4 -> U+E708
  ["eebf8e", :undef], # [PILE OF POO] U+E4F5 -> "[ウンチ]"
  ["eebf82", :undef], # [FLEXED BICEPS] U+E4E9 -> "[力こぶ]"
  ["eeb5a0", :undef], # [DIZZY SYMBOL] U+EB5C -> "[クラクラ]"
  ["eebf96", :undef], # [SPEECH BALLOON] U+E4FD -> "[フキダシ]"
  ["eeb1be", "ee9bba"], # [SPARKLES] U+EAAB -> U+E6FA
  ["eebd91", "ee9bb8"], # [EIGHT POINTED BLACK STAR] U+E479 -> U+E6F8
  ["ef819a", "ee9bb8"], # [EIGHT SPOKED ASTERISK] U+E53E -> U+E6F8
  ["ef8196", "ee9a9c"], # [MEDIUM WHITE CIRCLE] U+E53A -> U+E69C
  ["ef8197", "ee9a9c"], # [MEDIUM BLACK CIRCLE] U+E53B -> U+E69C
  ["ef81a6", "ee9a9c"], # [LARGE RED CIRCLE] U+E54A -> U+E69C
  ["ef81a7", "ee9a9c"], # [LARGE BLUE CIRCLE] U+E54B -> U+E69C
  ["eebda3", :undef], # [WHITE MEDIUM STAR] U+E48B -> "[☆]"
  ["ef81a4", :undef], # [WHITE LARGE SQUARE] U+E548 -> "■"
  ["ef81a5", :undef], # [BLACK LARGE SQUARE] U+E549 -> "■"
  ["ef818d", :undef], # [WHITE SMALL SQUARE] U+E531 -> "■"
  ["ef818e", :undef], # [BLACK SMALL SQUARE] U+E532 -> "■"
  ["ef8190", :undef], # [WHITE MEDIUM SMALL SQUARE] U+E534 -> "■"
  ["ef8191", :undef], # [BLACK MEDIUM SMALL SQUARE] U+E535 -> "■"
  ["ef8194", :undef], # [WHITE MEDIUM SQUARE] U+E538 -> "■"
  ["ef8195", :undef], # [BLACK MEDIUM SQUARE] U+E539 -> "■"
  ["ef81a2", :undef], # [LARGE ORANGE DIAMOND] U+E546 -> "◆"
  ["ef81a3", :undef], # [LARGE BLUE DIAMOND] U+E547 -> "◆"
  ["ef8192", :undef], # [SMALL ORANGE DIAMOND] U+E536 -> "◆"
  ["ef8193", :undef], # [SMALL BLUE DIAMOND] U+E537 -> "◆"
  ["eebd84", "ee9bba"], # [SPARKLE] U+E46C -> U+E6FA
  ["eebf89", :undef], # [WHITE FLOWER] U+E4F0 -> "[花丸]"
  ["eebf8b", :undef], # [HUNDRED POINTS SYMBOL] U+E4F2 -> "[100点]"
  ["ef81b9", "ee9b9a"], # [LEFTWARDS ARROW WITH HOOK] U+E55D -> U+E6DA
  ["ef81b8", :undef], # [RIGHTWARDS ARROW WITH HOOK] U+E55C -> "└→"
  ["ef83a4", "ee9cb5"], # [CLOCKWISE DOWNWARDS AND UPWARDS OPEN CIRCLE ARROWS] U+EB0D -> U+E735
  ["eebfaa", :undef], # [SPEAKER WITH THREE SOUND WAVES] U+E511 -> "[スピーカ]"
  ["ef82a1", :undef], # [BATTERY] U+E584 -> "[電池]"
  ["ef82a6", :undef], # [ELECTRIC PLUG] U+E589 -> "[コンセント]"
  ["eebfb1", "ee9b9c"], # [LEFT-POINTING MAGNIFYING GLASS] U+E518 -> U+E6DC
  ["ef839c", "ee9b9c"], # [RIGHT-POINTING MAGNIFYING GLASS] U+EB05 -> U+E6DC
  ["eebfb5", "ee9b99"], # [LOCK] U+E51C -> U+E6D9
  ["ef83a3", "ee9b99"], # [LOCK WITH INK PEN] U+EB0C -> U+E6D9
  ["ef8393", "ee9b99"], # [CLOSED LOCK WITH KEY] U+EAFC -> U+E6D9
  ["eebfb2", "ee9b99"], # [KEY] U+E519 -> U+E6D9
  ["eebfab", "ee9c93"], # [BELL] U+E512 -> U+E713
  ["ef8399", :undef], # [BALLOT BOX WITH CHECK] U+EB02 -> "[チェックマーク]"
  ["ef839b", :undef], # [RADIO BUTTON] U+EB04 -> "[ラジオボタン]"
  ["ef839e", :undef], # [BOOKMARK] U+EB07 -> "[ブックマーク]"
  ["ef82a7", :undef], # [LINK SYMBOL] U+E58A -> "[リンク]"
  ["ef839d", :undef], # [BACK WITH LEFTWARDS ARROW ABOVE] U+EB06 -> "[←BACK]"
  ["ef82a9", :undef], # [EM SPACE] U+E58C -> U+3013 (GETA)
  ["ef82aa", :undef], # [EN SPACE] U+E58D -> U+3013 (GETA)
  ["ef82ab", :undef], # [FOUR-PER-EM SPACE] U+E58E -> U+3013 (GETA)
  ["ef81ba", :undef], # [WHITE HEAVY CHECK MARK] U+E55E -> "[チェックマーク]"
  ["eeb688", "ee9a93"], # [RAISED FIST] U+EB83 -> U+E693
  ["ef8384", "ee9a95"], # [RAISED HAND] U+E5A7 -> U+E695
  ["ef8383", "ee9a94"], # [VICTORY HAND] U+E5A6 -> U+E694
  ["eebf8c", "ee9bbd"], # [FISTED HAND SIGN] U+E4F3 -> U+E6FD
  ["eebf92", "ee9ca7"], # [THUMBS UP SIGN] U+E4F9 -> U+E727
  ["eebf8f", :undef], # [WHITE UP POINTING INDEX] U+E4F6 -> "[人差し指]"
  ["eeb1a0", :undef], # [WHITE UP POINTING BACKHAND INDEX] U+EA8D -> "[↑]"
  ["eeb1a1", :undef], # [WHITE DOWN POINTING BACKHAND INDEX] U+EA8E -> "[↓]"
  ["eebf98", :undef], # [WHITE LEFT POINTING BACKHAND INDEX] U+E4FF -> "[←]"
  ["eebf99", :undef], # [WHITE RIGHT POINTING BACKHAND INDEX] U+E500 -> "[→]"
  ["eeb2aa", "ee9a95"], # [WAVING HAND SIGN] U+EAD6 -> U+E695
  ["eeb2a7", :undef], # [CLAPPING HANDS SIGN] U+EAD3 -> "[拍手]"
  ["eeb2a8", "ee9c8b"], # [OK HAND SIGN] U+EAD4 -> U+E70B
  ["eeb2a9", "ee9c80"], # [THUMBS DOWN SIGN] U+EAD5 -> U+E700
  ["ef8294", :undef], # [EMOJI COMPATIBILITY SYMBOL-37] U+E577 -> "[EZ]"
  ["ef838f", :undef], # [EMOJI COMPATIBILITY SYMBOL-38] U+E5B2 -> "[ezplus]"
  ["eeb1b0", :undef], # [EMOJI COMPATIBILITY SYMBOL-39] U+EA9D -> "[EZナビ]"
  ["eeb5b8", :undef], # [EMOJI COMPATIBILITY SYMBOL-40] U+EB74 -> "[EZムービー]"
  ["eeb686", :undef], # [EMOJI COMPATIBILITY SYMBOL-41] U+EB81 -> "[Cメール]"
  ["eeb68e", :undef], # [EMOJI COMPATIBILITY SYMBOL-42] U+EB89 -> "[Java]"
  ["eeb68f", :undef], # [EMOJI COMPATIBILITY SYMBOL-43] U+EB8A -> "[BREW]"
  ["eeb690", :undef], # [EMOJI COMPATIBILITY SYMBOL-44] U+EB8B -> "[EZ着うた]"
  ["eeb691", :undef], # [EMOJI COMPATIBILITY SYMBOL-45] U+EB8C -> "[EZナビ]"
  ["eeb692", :undef], # [EMOJI COMPATIBILITY SYMBOL-46] U+EB8D -> "[WIN]"
  ["eeb693", :undef], # [EMOJI COMPATIBILITY SYMBOL-47] U+EB8E -> "[プレミアム]"
  ["ef8392", :undef], # [EMOJI COMPATIBILITY SYMBOL-48] U+EAFB -> "[オープンウェブ]"
  ["ef82a3", :undef], # [EMOJI COMPATIBILITY SYMBOL-49] U+E586 -> "[PDC]"
  ["ef8188", "ee9ba1"], # [EMOJI COMPATIBILITY SYMBOL-66] U+E52C -> U+E6E1
]

EMOJI_EXCHANGE_TBL['UTF8-KDDI']['UTF8-SoftBank'] = [
  # for documented codepoints
  ["ee9288", "ee818a"], # [BLACK SUN WITH RAYS] U+E488 -> U+E04A
  ["ee928d", "ee8189"], # [CLOUD] U+E48D -> U+E049
  ["ee928c", "ee818b"], # [UMBRELLA WITH RAIN DROPS] U+E48C -> U+E04B
  ["ee9285", "ee8188"], # [SNOWMAN WITHOUT SNOW] U+E485 -> U+E048
  ["ee9287", "ee84bd"], # [HIGH VOLTAGE SIGN] U+E487 -> U+E13D
  ["ee91a9", "ee9183"], # [CYCLONE] U+E469 -> U+E443
  ["ee9698", :undef], # [FOGGY] U+E598 -> "[霧]"
  ["eeaba8", "ee90bc"], # [CLOSED UMBRELLA] U+EAE8 -> U+E43C
  ["eeabb1", "ee918b"], # [NIGHT WITH STARS] U+EAF1 -> U+E44B
  ["eeabb4", "ee9189"], # [SUNRISE] U+EAF4 -> U+E449
  ["ee979a", "ee8586"], # [CITYSCAPE AT DUSK] U+E5DA -> U+E146
  ["eeabb2", "ee918c"], # [RAINBOW] U+EAF2 -> U+E44C
  ["ee928a", :undef], # [SNOWFLAKE] U+E48A -> "[雪結晶]"
  ["ee928e", "ee818aee8189"], # [SUN BEHIND CLOUD] U+E48E -> U+E04A U+E049
  ["ee92bf", "ee918b"], # [BRIDGE AT NIGHT] U+E4BF -> U+E44B
  ["eeadbc", "ee90be"], # [WATER WAVE] U+EB7C -> U+E43E
  ["eead93", :undef], # [VOLCANO] U+EB53 -> "[火山]"
  ["eead9f", "ee918b"], # [MILKY WAY] U+EB5F -> U+E44B
  ["ee96b3", :undef], # [EARTH GLOBE ASIA-AUSTRALIA] U+E5B3 -> "[地球]"
  ["ee96a8", :undef], # [NEW MOON SYMBOL] U+E5A8 -> "●"
  ["ee96a9", "ee818c"], # [WAXING GIBBOUS MOON SYMBOL] U+E5A9 -> U+E04C
  ["ee96aa", "ee818c"], # [FIRST QUARTER MOON SYMBOL] U+E5AA -> U+E04C
  ["ee9286", "ee818c"], # [CRESCENT MOON] U+E486 -> U+E04C
  ["ee9289", "ee818c"], # [FIRST QUARTER MOON WITH FACE] U+E489 -> U+E04C
  ["ee91a8", :undef], # [SHOOTING STAR] U+E468 -> "☆彡"
  ["ee95ba", :undef], # [WATCH] U+E57A -> "[腕時計]"
  ["ee95bb", :undef], # [HOURGLASS] U+E57B -> "[砂時計]"
  ["ee9694", "ee80ad"], # [ALARM CLOCK] U+E594 -> U+E02D
  ["ee91bc", :undef], # [HOURGLASS WITH FLOWING SAND] U+E47C -> "[砂時計]"
  ["ee928f", "ee88bf"], # [ARIES] U+E48F -> U+E23F
  ["ee9290", "ee8980"], # [TAURUS] U+E490 -> U+E240
  ["ee9291", "ee8981"], # [GEMINI] U+E491 -> U+E241
  ["ee9292", "ee8982"], # [CANCER] U+E492 -> U+E242
  ["ee9293", "ee8983"], # [LEO] U+E493 -> U+E243
  ["ee9294", "ee8984"], # [VIRGO] U+E494 -> U+E244
  ["ee9295", "ee8985"], # [LIBRA] U+E495 -> U+E245
  ["ee9296", "ee8986"], # [SCORPIUS] U+E496 -> U+E246
  ["ee9297", "ee8987"], # [SAGITTARIUS] U+E497 -> U+E247
  ["ee9298", "ee8988"], # [CAPRICORN] U+E498 -> U+E248
  ["ee9299", "ee8989"], # [AQUARIUS] U+E499 -> U+E249
  ["ee929a", "ee898a"], # [PISCES] U+E49A -> U+E24A
  ["ee929b", "ee898b"], # [OPHIUCHUS] U+E49B -> U+E24B
  ["ee9493", "ee8490"], # [FOUR LEAF CLOVER] U+E513 -> U+E110
  ["ee93a4", "ee8c84"], # [TULIP] U+E4E4 -> U+E304
  ["eeadbd", "ee8490"], # [SEEDLING] U+EB7D -> U+E110
  ["ee938e", "ee8498"], # [MAPLE LEAF] U+E4CE -> U+E118
  ["ee938a", "ee80b0"], # [CHERRY BLOSSOM] U+E4CA -> U+E030
  ["ee96ba", "ee80b2"], # [ROSE] U+E5BA -> U+E032
  ["ee978d", "ee8499"], # [FALLEN LEAF] U+E5CD -> U+E119
  ["eeaa94", "ee8c83"], # [HIBISCUS] U+EA94 -> U+E303
  ["ee93a3", "ee8c85"], # [SUNFLOWER] U+E4E3 -> U+E305
  ["ee93a2", "ee8c87"], # [PALM TREE] U+E4E2 -> U+E307
  ["eeaa96", "ee8c88"], # [CACTUS] U+EA96 -> U+E308
  ["eeacb6", :undef], # [EAR OF MAIZE] U+EB36 -> "[とうもろこし]"
  ["eeacb7", :undef], # [MUSHROOM] U+EB37 -> "[キノコ]"
  ["eeacb8", :undef], # [CHESTNUT] U+EB38 -> "[栗]"
  ["eead89", "ee8c85"], # [BLOSSOM] U+EB49 -> U+E305
  ["eeae82", "ee8490"], # [HERB] U+EB82 -> U+E110
  ["ee9392", :undef], # [CHERRIES] U+E4D2 -> "[さくらんぼ]"
  ["eeacb5", :undef], # [BANANA] U+EB35 -> "[バナナ]"
  ["eeaab9", "ee8d85"], # [RED APPLE] U+EAB9 -> U+E345
  ["eeaaba", "ee8d86"], # [TANGERINE] U+EABA -> U+E346
  ["ee9394", "ee8d87"], # [STRAWBERRY] U+E4D4 -> U+E347
  ["ee938d", "ee8d88"], # [WATERMELON] U+E4CD -> U+E348
  ["eeaabb", "ee8d89"], # [TOMATO] U+EABB -> U+E349
  ["eeaabc", "ee8d8a"], # [AUBERGINE] U+EABC -> U+E34A
  ["eeacb2", :undef], # [MELON] U+EB32 -> "[メロン]"
  ["eeacb3", :undef], # [PINEAPPLE] U+EB33 -> "[パイナップル]"
  ["eeacb4", :undef], # [GRAPES] U+EB34 -> "[ブドウ]"
  ["eeacb9", :undef], # [PEACH] U+EB39 -> "[モモ]"
  ["eead9a", "ee8d85"], # [GREEN APPLE] U+EB5A -> U+E345
  ["ee96a4", "ee9099"], # [EYES] U+E5A4 -> U+E419
  ["ee96a5", "ee909b"], # [EAR] U+E5A5 -> U+E41B
  ["eeab90", "ee909a"], # [NOSE] U+EAD0 -> U+E41A
  ["eeab91", "ee909c"], # [MOUTH] U+EAD1 -> U+E41C
  ["eead87", "ee9089"], # [TONGUE] U+EB47 -> U+E409
  ["ee9489", "ee8c9c"], # [LIPSTICK] U+E509 -> U+E31C
  ["eeaaa0", "ee8c9d"], # [NAIL POLISH] U+EAA0 -> U+E31D
  ["ee948b", "ee8c9e"], # [FACE MASSAGE] U+E50B -> U+E31E
  ["eeaaa1", "ee8c9f"], # [HAIRCUT] U+EAA1 -> U+E31F
  ["eeaaa2", "ee8ca0"], # [BARBER POLE] U+EAA2 -> U+E320
  ["ee93bc", "ee8081"], # [BOY] U+E4FC -> U+E001
  ["ee93ba", "ee8082"], # [GIRL] U+E4FA -> U+E002
  ["ee9481", :undef], # [FAMILY] U+E501 -> "[家族]"
  ["ee979d", "ee8592"], # [POLICE OFFICER] U+E5DD -> U+E152
  ["eeab9b", "ee90a9"], # [WOMAN WITH BUNNY EARS] U+EADB -> U+E429
  ["eeaba9", :undef], # [BRIDE WITH VEIL] U+EAE9 -> "[花嫁]"
  ["eeac93", "ee9495"], # [WESTERN PERSON] U+EB13 -> U+E515
  ["eeac94", "ee9496"], # [MAN WITH GUA PI MAO] U+EB14 -> U+E516
  ["eeac95", "ee9497"], # [MAN WITH TURBAN] U+EB15 -> U+E517
  ["eeac96", "ee9498"], # [OLDER MAN] U+EB16 -> U+E518
  ["eeac97", "ee9499"], # [OLDER WOMAN] U+EB17 -> U+E519
  ["eeac98", "ee949a"], # [BABY] U+EB18 -> U+E51A
  ["eeac99", "ee949b"], # [CONSTRUCTION WORKER] U+EB19 -> U+E51B
  ["eeac9a", "ee949c"], # [PRINCESS] U+EB1A -> U+E51C
  ["eead84", :undef], # [JAPANESE OGRE] U+EB44 -> "[なまはげ]"
  ["eead85", :undef], # [JAPANESE GOBLIN] U+EB45 -> "[天狗]"
  ["ee938b", "ee849b"], # [GHOST] U+E4CB -> U+E11B
  ["ee96bf", "ee818e"], # [BABY ANGEL] U+E5BF -> U+E04E
  ["ee948e", "ee848c"], # [EXTRATERRESTRIAL ALIEN] U+E50E -> U+E10C
  ["ee93ac", "ee84ab"], # [ALIEN MONSTER] U+E4EC -> U+E12B
  ["ee93af", "ee849a"], # [IMP] U+E4EF -> U+E11A
  ["ee93b8", "ee849c"], # [SKULL] U+E4F8 -> U+E11C
  ["eeac9c", "ee949f"], # [DANCER] U+EB1C -> U+E51F
  ["eeadbe", :undef], # [SNAIL] U+EB7E -> "[カタツムリ]"
  ["eeaca2", "ee94ad"], # [SNAKE] U+EB22 -> U+E52D
  ["eeaca3", "ee94ae"], # [CHICKEN] U+EB23 -> U+E52E
  ["eeaca4", "ee94af"], # [BOAR] U+EB24 -> U+E52F
  ["eeaca5", "ee94b0"], # [BACTRIAN CAMEL] U+EB25 -> U+E530
  ["eeac9f", "ee94a6"], # [ELEPHANT] U+EB1F -> U+E526
  ["eeaca0", "ee94a7"], # [KOALA] U+EB20 -> U+E527
  ["ee9787", "ee848a"], # [OCTOPUS] U+E5C7 -> U+E10A
  ["eeabac", "ee9181"], # [SPIRAL SHELL] U+EAEC -> U+E441
  ["eeac9e", "ee94a5"], # [BUG] U+EB1E -> U+E525
  ["ee939d", :undef], # [ANT] U+E4DD -> "[アリ]"
  ["eead97", :undef], # [HONEYBEE] U+EB57 -> "[ミツバチ]"
  ["eead98", :undef], # [LADY BEETLE] U+EB58 -> "[てんとう虫]"
  ["eeac9d", "ee94a2"], # [TROPICAL FISH] U+EB1D -> U+E522
  ["ee9393", "ee8099"], # [BLOWFISH] U+E4D3 -> U+E019
  ["ee9794", :undef], # [TURTLE] U+E5D4 -> "[カメ]"
  ["ee93a0", "ee94a3"], # [BABY CHICK] U+E4E0 -> U+E523
  ["eeadb6", "ee94a3"], # [FRONT-FACING BABY CHICK] U+EB76 -> U+E523
  ["ee979b", "ee94a3"], # [HATCHING CHICK] U+E5DB -> U+E523
  ["ee939c", "ee8195"], # [PENGUIN] U+E4DC -> U+E055
  ["ee939f", "ee8192"], # [POODLE] U+E4DF -> U+E052
  ["eeac9b", "ee94a0"], # [DOLPHIN] U+EB1B -> U+E520
  ["ee9782", "ee8193"], # [MOUSE FACE] U+E5C2 -> U+E053
  ["ee9780", "ee8190"], # [TIGER FACE] U+E5C0 -> U+E050
  ["ee939b", "ee818f"], # [CAT FACE] U+E4DB -> U+E04F
  ["ee91b0", "ee8194"], # [SPOUTING WHALE] U+E470 -> U+E054
  ["ee9398", "ee809a"], # [HORSE FACE] U+E4D8 -> U+E01A
  ["ee9399", "ee8489"], # [MONKEY FACE] U+E4D9 -> U+E109
  ["ee93a1", "ee8192"], # [DOG FACE] U+E4E1 -> U+E052
  ["ee939e", "ee848b"], # [PIG FACE] U+E4DE -> U+E10B
  ["ee9781", "ee8191"], # [BEAR FACE] U+E5C1 -> U+E051
  ["eeaca1", "ee94ab"], # [COW FACE] U+EB21 -> U+E52B
  ["ee9397", "ee94ac"], # [RABBIT FACE] U+E4D7 -> U+E52C
  ["ee939a", "ee94b1"], # [FROG FACE] U+E4DA -> U+E531
  ["ee93ae", "ee94b6"], # [PAW PRINTS] U+E4EE -> U+E536
  ["eeacbf", :undef], # [DRAGON FACE] U+EB3F -> "[辰]"
  ["eead86", :undef], # [PANDA FACE] U+EB46 -> "[パンダ]"
  ["eead88", "ee848b"], # [PIG NOSE] U+EB48 -> U+E10B
  ["ee91b2", "ee8199"], # [ANGRY FACE] U+E472 -> U+E059
  ["eeada7", "ee9083"], # [ANGUISHED FACE] U+EB67 -> U+E403
  ["eeab8a", "ee9090"], # [ASTONISHED FACE] U+EACA -> U+E410
  ["ee96ae", "ee9086"], # [DIZZY FACE] U+E5AE -> U+E406
  ["eeab8b", "ee908f"], # [EXASPERATED FACE] U+EACB -> U+E40F
  ["eeab89", "ee908e"], # [EXPRESSIONLESS FACE] U+EAC9 -> U+E40E
  ["ee9784", "ee8486"], # [FACE WITH HEART-SHAPED EYES] U+E5C4 -> U+E106
  ["eeab81", "ee9084"], # [FACE WITH LOOK OF TRIUMPH] U+EAC1 -> U+E404
  ["ee93a7", "ee8485"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E4E7 -> U+E105
  ["eeab8f", "ee9098"], # [FACE THROWING A KISS] U+EACF -> U+E418
  ["eeab8e", "ee9097"], # [FACE KISSING] U+EACE -> U+E417
  ["eeab87", "ee908c"], # [FACE WITH MASK] U+EAC7 -> U+E40C
  ["eeab88", "ee908d"], # [FLUSHED FACE] U+EAC8 -> U+E40D
  ["ee91b1", "ee8197"], # [HAPPY FACE WITH OPEN MOUTH] U+E471 -> U+E057
  ["eeae80", "ee9084"], # [HAPPY FACE WITH GRIN] U+EB80 -> U+E404
  ["eeada4", "ee9092"], # [HAPPY AND CRYING FACE] U+EB64 -> U+E412
  ["eeab8d", "ee8196"], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+EACD -> U+E056
  ["ee93bb", "ee9094"], # [WHITE SMILING FACE] U+E4FB -> U+E414
  ["eeada9", "ee9093"], # [CRYING FACE] U+EB69 -> U+E413
  ["ee91b3", "ee9091"], # [LOUDLY CRYING FACE] U+E473 -> U+E411
  ["eeab86", "ee908b"], # [FEARFUL FACE] U+EAC6 -> U+E40B
  ["eeab82", "ee9086"], # [PERSEVERING FACE] U+EAC2 -> U+E406
  ["eead9d", "ee9096"], # [POUTING FACE] U+EB5D -> U+E416
  ["eeab85", "ee908a"], # [RELIEVED FACE] U+EAC5 -> U+E40A
  ["eeab83", "ee9087"], # [CONFOUNDED FACE] U+EAC3 -> U+E407
  ["eeab80", "ee9083"], # [PENSIVE FACE] U+EAC0 -> U+E403
  ["ee9785", "ee8487"], # [FACE SCREAMING IN FEAR] U+E5C5 -> U+E107
  ["eeab84", "ee9088"], # [SLEEPY FACE] U+EAC4 -> U+E408
  ["eeaabf", "ee9082"], # [SMIRKING FACE] U+EABF -> U+E402
  ["ee9786", "ee8488"], # [FACE WITH COLD SWEAT] U+E5C6 -> U+E108
  ["ee91b4", "ee9086"], # [TIRED FACE] U+E474 -> U+E406
  ["ee9783", "ee9085"], # [WINKING FACE] U+E5C3 -> U+E405
  ["eeada1", "ee8197"], # [CAT FACE WITH OPEN MOUTH] U+EB61 -> U+E057
  ["eeadbf", "ee9084"], # [HAPPY CAT FACE WITH GRIN] U+EB7F -> U+E404
  ["eeada3", "ee9092"], # [HAPPY AND CRYING CAT FACE] U+EB63 -> U+E412
  ["eeada0", "ee9098"], # [CAT FACE KISSING] U+EB60 -> U+E418
  ["eeada5", "ee8486"], # [CAT FACE WITH HEART-SHAPED EYES] U+EB65 -> U+E106
  ["eeada8", "ee9093"], # [CRYING CAT FACE] U+EB68 -> U+E413
  ["eead9e", "ee9096"], # [POUTING CAT FACE] U+EB5E -> U+E416
  ["eeadaa", "ee9084"], # [CAT FACE WITH TIGHTLY-CLOSED LIPS] U+EB6A -> U+E404
  ["eeada6", "ee9083"], # [ANGUISHED CAT FACE] U+EB66 -> U+E403
  ["eeab97", "ee90a3"], # [FACE WITH NO GOOD GESTURE] U+EAD7 -> U+E423
  ["eeab98", "ee90a4"], # [FACE WITH OK GESTURE] U+EAD8 -> U+E424
  ["eeab99", "ee90a6"], # [PERSON BOWING DEEPLY] U+EAD9 -> U+E426
  ["eead90", :undef], # [SEE-NO-EVIL MONKEY] U+EB50 -> "(/_＼)"
  ["eead91", :undef], # [SPEAK-NO-EVIL MONKEY] U+EB51 -> "(・×・)"
  ["eead92", :undef], # [HEAR-NO-EVIL MONKEY] U+EB52 -> "|(・×・)|"
  ["eeae85", "ee8092"], # [PERSON RAISING ONE HAND] U+EB85 -> U+E012
  ["eeae86", "ee90a7"], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+EB86 -> U+E427
  ["eeae87", "ee9083"], # [PERSON FROWNING] U+EB87 -> U+E403
  ["eeae88", "ee9096"], # [PERSON WITH POUTING FACE] U+EB88 -> U+E416
  ["eeab92", "ee909d"], # [PERSON WITH FOLDED HANDS] U+EAD2 -> U+E41D
  ["ee92ab", "ee80b6"], # [HOUSE BUILDING] U+E4AB -> U+E036
  ["eeac89", "ee80b6"], # [HOUSE WITH GARDEN] U+EB09 -> U+E036
  ["ee92ad", "ee80b8"], # [OFFICE BUILDING] U+E4AD -> U+E038
  ["ee979e", "ee8593"], # [JAPANESE POST OFFICE] U+E5DE -> U+E153
  ["ee979f", "ee8595"], # [HOSPITAL] U+E5DF -> U+E155
  ["ee92aa", "ee858d"], # [BANK] U+E4AA -> U+E14D
  ["ee92a3", "ee8594"], # [AUTOMATED TELLER MACHINE] U+E4A3 -> U+E154
  ["eeaa81", "ee8598"], # [HOTEL] U+EA81 -> U+E158
  ["eeabb3", "ee9481"], # [LOVE HOTEL] U+EAF3 -> U+E501
  ["ee92a4", "ee8596"], # [CONVENIENCE STORE] U+E4A4 -> U+E156
  ["eeaa80", "ee8597"], # [SCHOOL] U+EA80 -> U+E157
  ["ee96bb", "ee80b7"], # [CHURCH] U+E5BB -> U+E037
  ["ee978f", "ee84a1"], # [FOUNTAIN] U+E5CF -> U+E121
  ["eeabb6", "ee9484"], # [DEPARTMENT STORE] U+EAF6 -> U+E504
  ["eeabb7", "ee9485"], # [JAPANESE CASTLE] U+EAF7 -> U+E505
  ["eeabb8", "ee9486"], # [EUROPEAN CASTLE] U+EAF8 -> U+E506
  ["eeabb9", "ee9488"], # [FACTORY] U+EAF9 -> U+E508
  ["ee92a9", "ee8882"], # [ANCHOR] U+E4A9 -> U+E202
  ["ee92bd", "ee8c8b"], # [IZAKAYA LANTERN] U+E4BD -> U+E30B
  ["ee96bd", "ee80bb"], # [MOUNT FUJI] U+E5BD -> U+E03B
  ["ee9380", "ee9489"], # [TOKYO TOWER] U+E4C0 -> U+E509
  ["ee95b2", :undef], # [SILHOUETTE OF JAPAN] U+E572 -> "[日本地図]"
  ["eeadac", :undef], # [MOYAI] U+EB6C -> "[モアイ]"
  ["ee96b7", "ee8087"], # [MANS SHOE] U+E5B7 -> U+E007
  ["eeacab", "ee8087"], # [ATHLETIC SHOE] U+EB2B -> U+E007
  ["ee949a", "ee84be"], # [HIGH-HEELED SHOE] U+E51A -> U+E13E
  ["eeaa9f", "ee8c9b"], # [WOMANS BOOTS] U+EA9F -> U+E31B
  ["eeacaa", "ee94b6"], # [FOOTPRINTS] U+EB2A -> U+E536
  ["ee93be", :undef], # [EYEGLASSES] U+E4FE -> "[メガネ]"
  ["ee96b6", "ee8086"], # [T-SHIRT] U+E5B6 -> U+E006
  ["eeadb7", :undef], # [JEANS] U+EB77 -> "[ジーンズ]"
  ["ee9789", "ee848e"], # [CROWN] U+E5C9 -> U+E10E
  ["eeaa93", "ee8c82"], # [NECKTIE] U+EA93 -> U+E302
  ["eeaa9e", "ee8c98"], # [WOMANS HAT] U+EA9E -> U+E318
  ["eeadab", "ee8c99"], # [DRESS] U+EB6B -> U+E319
  ["eeaaa3", "ee8ca1"], # [KIMONO] U+EAA3 -> U+E321
  ["eeaaa4", "ee8ca2"], # [BIKINI] U+EAA4 -> U+E322
  ["ee948d", "ee8086"], # [WOMANS CLOTHES] U+E50D -> U+E006
  ["ee9484", :undef], # [PURSE] U+E504 -> "[財布]"
  ["ee929c", "ee8ca3"], # [HANDBAG] U+E49C -> U+E323
  ["ee9387", "ee84af"], # [MONEY BAG] U+E4C7 -> U+E12F
  ["ee979c", "ee858a"], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+E5DC -> U+E14A
  ["ee95b9", "ee84af"], # [HEAVY DOLLAR SIGN] U+E579 -> U+E12F
  ["ee95bc", :undef], # [CREDIT CARD] U+E57C -> "[カード]"
  ["ee95bd", :undef], # [BANKNOTE WITH YEN SIGN] U+E57D -> "￥"
  ["ee9685", "ee84af"], # [BANKNOTE WITH DOLLAR SIGN] U+E585 -> U+E12F
  ["eead9b", :undef], # [MONEY WITH WINGS] U+EB5B -> "[飛んでいくお金]"
  ["eeac91", "ee9493"], # [REGIONAL INDICATOR SYMBOL LETTERS CN] U+EB11 -> U+E513
  ["eeac8e", "ee948e"], # [REGIONAL INDICATOR SYMBOL LETTERS DE] U+EB0E -> U+E50E
  ["ee9795", "ee9491"], # [REGIONAL INDICATOR SYMBOL LETTERS ES] U+E5D5 -> U+E511
  ["eeabba", "ee948d"], # [REGIONAL INDICATOR SYMBOL LETTERS FR] U+EAFA -> U+E50D
  ["eeac90", "ee9490"], # [REGIONAL INDICATOR SYMBOL LETTERS GB] U+EB10 -> U+E510
  ["eeac8f", "ee948f"], # [REGIONAL INDICATOR SYMBOL LETTERS IT] U+EB0F -> U+E50F
  ["ee938c", "ee948b"], # [REGIONAL INDICATOR SYMBOL LETTERS JP] U+E4CC -> U+E50B
  ["eeac92", "ee9494"], # [REGIONAL INDICATOR SYMBOL LETTERS KR] U+EB12 -> U+E514
  ["ee9796", "ee9492"], # [REGIONAL INDICATOR SYMBOL LETTERS RU] U+E5D6 -> U+E512
  ["ee95b3", "ee948c"], # [REGIONAL INDICATOR SYMBOL LETTERS US] U+E573 -> U+E50C
  ["ee91bb", "ee849d"], # [FIRE] U+E47B -> U+E11D
  ["ee9683", :undef], # [ELECTRIC TORCH] U+E583 -> "[懐中電灯]"
  ["ee9687", :undef], # [WRENCH] U+E587 -> "[レンチ]"
  ["ee978b", "ee8496"], # [HAMMER] U+E5CB -> U+E116
  ["ee9681", :undef], # [NUT AND BOLT] U+E581 -> "[ネジ]"
  ["ee95bf", :undef], # [HOCHO] U+E57F -> "[包丁]"
  ["ee948a", "ee8493"], # [PISTOL] U+E50A -> U+E113
  ["eeaa8f", "ee88be"], # [CRYSTAL BALL] U+EA8F -> U+E23E
  ["ee9280", "ee8889"], # [JAPANESE SYMBOL FOR BEGINNER] U+E480 -> U+E209
  ["ee9490", "ee84bb"], # [SYRINGE] U+E510 -> U+E13B
  ["eeaa9a", "ee8c8f"], # [PILL] U+EA9A -> U+E30F
  ["eeaca6", "ee94b2"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+EB26 -> U+E532
  ["eeaca7", "ee94b3"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+EB27 -> U+E533
  ["eeaca9", "ee94b4"], # [NEGATIVE SQUARED AB] U+EB29 -> U+E534
  ["eeaca8", "ee94b5"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+EB28 -> U+E535
  ["ee969f", "ee8c94"], # [RIBBON] U+E59F -> U+E314
  ["ee938f", "ee8492"], # [WRAPPED PRESENT] U+E4CF -> U+E112
  ["ee96a0", "ee8d8b"], # [BIRTHDAY CAKE] U+E5A0 -> U+E34B
  ["ee9389", "ee80b3"], # [CHRISTMAS TREE] U+E4C9 -> U+E033
  ["eeabb0", "ee9188"], # [FATHER CHRISTMAS] U+EAF0 -> U+E448
  ["ee9799", "ee8583"], # [CROSSED FLAGS] U+E5D9 -> U+E143
  ["ee978c", "ee8497"], # [FIREWORKS] U+E5CC -> U+E117
  ["eeaa9b", "ee8c90"], # [BALLOON] U+EA9B -> U+E310
  ["eeaa9c", "ee8c92"], # [PARTY POPPER] U+EA9C -> U+E312
  ["eeaba3", "ee90b6"], # [PINE DECORATION] U+EAE3 -> U+E436
  ["eeaba4", "ee90b8"], # [JAPANESE DOLLS] U+EAE4 -> U+E438
  ["eeaba5", "ee90b9"], # [GRADUATION CAP] U+EAE5 -> U+E439
  ["eeaba6", "ee90ba"], # [SCHOOL SATCHEL] U+EAE6 -> U+E43A
  ["eeaba7", "ee90bb"], # [CARP STREAMER] U+EAE7 -> U+E43B
  ["eeabab", "ee9180"], # [FIREWORK SPARKLER] U+EAEB -> U+E440
  ["eeabad", "ee9182"], # [WIND CHIME] U+EAED -> U+E442
  ["eeabae", "ee9185"], # [JACK-O-LANTERN] U+EAEE -> U+E445
  ["ee91af", :undef], # [CONFETTI BALL] U+E46F -> "[オメデトウ]"
  ["eeacbd", :undef], # [TANABATA TREE] U+EB3D -> "[七夕]"
  ["eeabaf", "ee9186"], # [MOON VIEWING CEREMONY] U+EAEF -> U+E446
  ["ee969b", :undef], # [PAGER] U+E59B -> "[ポケベル]"
  ["ee9696", "ee8089"], # [BLACK TELEPHONE] U+E596 -> U+E009
  ["ee949e", "ee8089"], # [TELEPHONE RECEIVER] U+E51E -> U+E009
  ["ee9688", "ee808a"], # [MOBILE PHONE] U+E588 -> U+E00A
  ["eeac88", "ee8484"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+EB08 -> U+E104
  ["eeaa92", "ee8c81"], # [MEMO] U+EA92 -> U+E301
  ["ee94a0", "ee808b"], # [FAX MACHINE] U+E520 -> U+E00B
  ["ee94a1", "ee8483"], # [ENVELOPE] U+E521 -> U+E103
  ["ee9691", "ee8483"], # [INCOMING ENVELOPE] U+E591 -> U+E103
  ["eeada2", "ee8483"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+EB62 -> U+E103
  ["ee949b", "ee8481"], # [CLOSED MAILBOX WITH LOWERED FLAG] U+E51B -> U+E101
  ["eeac8a", "ee8481"], # [CLOSED MAILBOX WITH RAISED FLAG] U+EB0A -> U+E101
  ["ee968b", :undef], # [NEWSPAPER] U+E58B -> "[新聞]"
  ["ee92a8", "ee858b"], # [SATELLITE ANTENNA] U+E4A8 -> U+E14B
  ["ee9692", :undef], # [OUTBOX TRAY] U+E592 -> "[送信BOX]"
  ["ee9693", :undef], # [INBOX TRAY] U+E593 -> "[受信BOX]"
  ["ee949f", "ee8492"], # [PACKAGE] U+E51F -> U+E112
  ["eeadb1", "ee8483"], # [E-MAIL SYMBOL] U+EB71 -> U+E103
  ["eeabbd", :undef], # [INPUT SYMBOL FOR LATIN CAPITAL LETTERS] U+EAFD -> "[ABCD]"
  ["eeabbe", :undef], # [INPUT SYMBOL FOR LATIN SMALL LETTERS] U+EAFE -> "[abcd]"
  ["eeabbf", :undef], # [INPUT SYMBOL FOR NUMBERS] U+EAFF -> "[1234]"
  ["eeac80", :undef], # [INPUT SYMBOL FOR SYMBOLS] U+EB00 -> "[記号]"
  ["eead95", :undef], # [INPUT SYMBOL FOR LATIN LETTERS] U+EB55 -> "[ABC]"
  ["eeac83", :undef], # [BLACK NIB] U+EB03 -> "[ペン]"
  ["ee96b8", "ee808c"], # [PERSONAL COMPUTER] U+E5B8 -> U+E00C
  ["ee92a1", "ee8c81"], # [PENCIL] U+E4A1 -> U+E301
  ["ee92a0", :undef], # [PAPERCLIP] U+E4A0 -> "[クリップ]"
  ["ee978e", "ee849e"], # [BRIEFCASE] U+E5CE -> U+E11E
  ["ee9682", "ee8c96"], # [MINIDISC] U+E582 -> U+E316
  ["ee95a2", "ee8c96"], # [FLOPPY DISK] U+E562 -> U+E316
  ["ee948c", "ee84a6"], # [OPTICAL DISC] U+E50C -> U+E126
  ["ee9496", "ee8c93"], # [BLACK SCISSORS] U+E516 -> U+E313
  ["ee95a0", :undef], # [ROUND PUSHPIN] U+E560 -> "[画びょう]"
  ["ee95a1", "ee8c81"], # [PAGE WITH CURL] U+E561 -> U+E301
  ["ee95a9", "ee8c81"], # [PAGE FACING UP] U+E569 -> U+E301
  ["ee95a3", :undef], # [CALENDAR] U+E563 -> "[カレンダー]"
  ["ee968f", :undef], # [FILE FOLDER] U+E58F -> "[フォルダ]"
  ["ee9690", :undef], # [OPEN FILE FOLDER] U+E590 -> "[フォルダ]"
  ["ee95ab", "ee8588"], # [NOTEBOOK] U+E56B -> U+E148
  ["ee929f", "ee8588"], # [OPEN BOOK] U+E49F -> U+E148
  ["ee929d", "ee8588"], # [NOTEBOOK WITH DECORATIVE COVER] U+E49D -> U+E148
  ["ee95a8", "ee8588"], # [CLOSED BOOK] U+E568 -> U+E148
  ["ee95a5", "ee8588"], # [GREEN BOOK] U+E565 -> U+E148
  ["ee95a6", "ee8588"], # [BLUE BOOK] U+E566 -> U+E148
  ["ee95a7", "ee8588"], # [ORANGE BOOK] U+E567 -> U+E148
  ["ee95af", "ee8588"], # [BOOKS] U+E56F -> U+E148
  ["ee949d", :undef], # [NAME BADGE] U+E51D -> "[名札]"
  ["ee959f", :undef], # [SCROLL] U+E55F -> "[スクロール]"
  ["ee95a4", "ee8c81"], # [CLIPBOARD] U+E564 -> U+E301
  ["ee95aa", :undef], # [TEAR-OFF CALENDAR] U+E56A -> "[カレンダー]"
  ["ee95b4", "ee858a"], # [BAR CHART] U+E574 -> U+E14A
  ["ee95b5", "ee858a"], # [CHART WITH UPWARDS TREND] U+E575 -> U+E14A
  ["ee95b6", :undef], # [CHART WITH DOWNWARDS TREND] U+E576 -> "[グラフ]"
  ["ee95ac", "ee8588"], # [CARD INDEX] U+E56C -> U+E148
  ["ee95ad", :undef], # [PUSHPIN] U+E56D -> "[画びょう]"
  ["ee95ae", "ee8588"], # [LEDGER] U+E56E -> U+E148
  ["ee95b0", :undef], # [STRAIGHT RULER] U+E570 -> "[定規]"
  ["ee92a2", :undef], # [TRIANGULAR RULER] U+E4A2 -> "[三角定規]"
  ["eeac8b", "ee8c81"], # [BOOKMARK TABS] U+EB0B -> U+E301
  ["ee92ba", "ee8096"], # [BASEBALL] U+E4BA -> U+E016
  ["ee9699", "ee8094"], # [FLAG IN HOLE] U+E599 -> U+E014
  ["ee92b7", "ee8095"], # [TENNIS RACQUET AND BALL] U+E4B7 -> U+E015
  ["ee92b6", "ee8098"], # [SOCCER BALL] U+E4B6 -> U+E018
  ["eeaaac", "ee8093"], # [SKI AND SKI BOOT] U+EAAC -> U+E013
  ["ee969a", "ee90aa"], # [BASKETBALL AND HOOP] U+E59A -> U+E42A
  ["ee92b9", "ee84b2"], # [CHEQUERED FLAG] U+E4B9 -> U+E132
  ["ee92b8", :undef], # [SNOWBOARDER] U+E4B8 -> "[スノボ]"
  ["ee91ab", "ee8495"], # [RUNNER] U+E46B -> U+E115
  ["eead81", "ee8097"], # [SURFER] U+EB41 -> U+E017
  ["ee9793", "ee84b1"], # [TROPHY] U+E5D3 -> U+E131
  ["ee92bb", "ee90ab"], # [AMERICAN FOOTBALL] U+E4BB -> U+E42B
  ["eeab9e", "ee90ad"], # [SWIMMER] U+EADE -> U+E42D
  ["ee92b5", "ee809e"], # [TRAIN] U+E4B5 -> U+E01E
  ["ee96bc", "ee90b4"], # [METRO] U+E5BC -> U+E434
  ["ee92b0", "ee809f"], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+E4B0 -> U+E01F
  ["ee92b1", "ee809b"], # [AUTOMOBILE] U+E4B1 -> U+E01B
  ["ee92af", "ee8599"], # [ONCOMING BUS] U+E4AF -> U+E159
  ["ee92a7", "ee8590"], # [BUS STOP] U+E4A7 -> U+E150
  ["eeaa82", "ee8882"], # [SHIP] U+EA82 -> U+E202
  ["ee92b3", "ee809d"], # [AIRPLANE] U+E4B3 -> U+E01D
  ["ee92b4", "ee809c"], # [SAILBOAT] U+E4B4 -> U+E01C
  ["eeadad", "ee80b9"], # [STATION] U+EB6D -> U+E039
  ["ee9788", "ee848d"], # [ROCKET] U+E5C8 -> U+E10D
  ["ee92b2", "ee90af"], # [DELIVERY TRUCK] U+E4B2 -> U+E42F
  ["eeab9f", "ee90b0"], # [FIRE ENGINE] U+EADF -> U+E430
  ["eeaba0", "ee90b1"], # [AMBULANCE] U+EAE0 -> U+E431
  ["eeaba1", "ee90b2"], # [POLICE CAR] U+EAE1 -> U+E432
  ["ee95b1", "ee80ba"], # [FUEL PUMP] U+E571 -> U+E03A
  ["ee92a6", "ee858f"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E4A6 -> U+E14F
  ["ee91aa", "ee858e"], # [HORIZONTAL TRAFFIC LIGHT] U+E46A -> U+E14E
  ["ee9797", "ee84b7"], # [CONSTRUCTION SIGN] U+E5D7 -> U+E137
  ["eeadb3", "ee90b2"], # [POLICE CARS REVOLVING LIGHT] U+EB73 -> U+E432
  ["ee92bc", "ee84a3"], # [HOT SPRINGS] U+E4BC -> U+E123
  ["ee9790", "ee84a2"], # [TENT] U+E5D0 -> U+E122
  ["ee91ad", "ee84a4"], # [FERRIS WHEEL] U+E46D -> U+E124
  ["eeaba2", "ee90b3"], # [ROLLER COASTER] U+EAE2 -> U+E433
  ["eead82", "ee8099"], # [FISHING POLE AND FISH] U+EB42 -> U+E019
  ["ee9483", "ee80bc"], # [MICROPHONE] U+E503 -> U+E03C
  ["ee9497", "ee80bd"], # [MOVIE CAMERA] U+E517 -> U+E03D
  ["ee9488", "ee8c8a"], # [HEADPHONE] U+E508 -> U+E30A
  ["ee969c", "ee9482"], # [ARTIST PALETTE] U+E59C -> U+E502
  ["eeabb5", "ee9483"], # [TOP HAT] U+EAF5 -> U+E503
  ["ee969e", :undef], # [CIRCUS TENT] U+E59E -> "[イベント]"
  ["ee929e", "ee84a5"], # [TICKET] U+E49E -> U+E125
  ["ee92be", "ee8ca4"], # [CLAPPER BOARD] U+E4BE -> U+E324
  ["ee969d", "ee9483"], # [PERFORMING ARTS] U+E59D -> U+E503
  ["ee9386", :undef], # [VIDEO GAME] U+E4C6 -> "[ゲーム]"
  ["ee9791", "ee84ad"], # [MAHJONG TILE RED DRAGON] U+E5D1 -> U+E12D
  ["ee9385", "ee84b0"], # [DIRECT HIT] U+E4C5 -> U+E130
  ["ee91ae", "ee84b3"], # [SLOT MACHINE] U+E46E -> U+E133
  ["eeab9d", "ee90ac"], # [BILLIARDS] U+EADD -> U+E42C
  ["ee9388", :undef], # [GAME DIE] U+E4C8 -> "[サイコロ]"
  ["eead83", :undef], # [BOWLING] U+EB43 -> "[ボーリング]"
  ["eeadae", :undef], # [FLOWER PLAYING CARDS] U+EB6E -> "[花札]"
  ["eeadaf", :undef], # [PLAYING CARD BLACK JOKER] U+EB6F -> "[ジョーカー]"
  ["ee96be", "ee80be"], # [MUSICAL NOTE] U+E5BE -> U+E03E
  ["ee9485", "ee8ca6"], # [MULTIPLE MUSICAL NOTES] U+E505 -> U+E326
  ["ee9486", "ee8181"], # [GUITAR] U+E506 -> U+E041
  ["eead80", :undef], # [MUSICAL KEYBOARD] U+EB40 -> "[ピアノ]"
  ["eeab9c", "ee8182"], # [TRUMPET] U+EADC -> U+E042
  ["ee9487", :undef], # [VIOLIN] U+E507 -> "[バイオリン]"
  ["eeab8c", "ee8ca6"], # [MUSICAL SCORE] U+EACC -> U+E326
  ["ee9495", "ee8088"], # [CAMERA] U+E515 -> U+E008
  ["ee95be", "ee80bd"], # [VIDEO CAMERA] U+E57E -> U+E03D
  ["ee9482", "ee84aa"], # [TELEVISION] U+E502 -> U+E12A
  ["ee96b9", "ee84a8"], # [RADIO] U+E5B9 -> U+E128
  ["ee9680", "ee84a9"], # [VIDEOCASSETTE] U+E580 -> U+E129
  ["ee93ab", "ee8083"], # [KISS MARK] U+E4EB -> U+E003
  ["eeadb8", "ee8483ee8ca8"], # [LOVE LETTER] U+EB78 -> U+E103 U+E328
  ["ee9494", "ee80b4"], # [RING] U+E514 -> U+E034
  ["ee978a", "ee8491"], # [KISS] U+E5CA -> U+E111
  ["eeaa95", "ee8c86"], # [BOUQUET] U+EA95 -> U+E306
  ["eeab9a", "ee90a5"], # [COUPLE WITH HEART] U+EADA -> U+E425
  ["eeaa83", "ee8887"], # [NO ONE UNDER EIGHTEEN SYMBOL] U+EA83 -> U+E207
  ["ee9598", "ee898e"], # [COPYRIGHT SIGN] U+E558 -> U+E24E
  ["ee9599", "ee898f"], # [REGISTERED SIGN] U+E559 -> U+E24F
  ["ee958e", "ee94b7"], # [TRADE MARK SIGN] U+E54E -> U+E537
  ["ee94b3", :undef], # [INFORMATION SOURCE] U+E533 -> "[ｉ]"
  ["eeae84", "ee8890"], # [HASH KEY] U+EB84 -> U+E210
  ["ee94a2", "ee889c"], # [KEYCAP 1] U+E522 -> U+E21C
  ["ee94a3", "ee889d"], # [KEYCAP 2] U+E523 -> U+E21D
  ["ee94a4", "ee889e"], # [KEYCAP 3] U+E524 -> U+E21E
  ["ee94a5", "ee889f"], # [KEYCAP 4] U+E525 -> U+E21F
  ["ee94a6", "ee88a0"], # [KEYCAP 5] U+E526 -> U+E220
  ["ee94a7", "ee88a1"], # [KEYCAP 6] U+E527 -> U+E221
  ["ee94a8", "ee88a2"], # [KEYCAP 7] U+E528 -> U+E222
  ["ee94a9", "ee88a3"], # [KEYCAP 8] U+E529 -> U+E223
  ["ee94aa", "ee88a4"], # [KEYCAP 9] U+E52A -> U+E224
  ["ee96ac", "ee88a5"], # [KEYCAP 0] U+E5AC -> U+E225
  ["ee94ab", :undef], # [KEYCAP TEN] U+E52B -> "[10]"
  ["eeaa84", "ee888b"], # [ANTENNA WITH BARS] U+EA84 -> U+E20B
  ["eeaa90", "ee8990"], # [VIBRATION MODE] U+EA90 -> U+E250
  ["eeaa91", "ee8991"], # [MOBILE PHONE OFF] U+EA91 -> U+E251
  ["ee9396", "ee84a0"], # [HAMBURGER] U+E4D6 -> U+E120
  ["ee9395", "ee8d82"], # [RICE BALL] U+E4D5 -> U+E342
  ["ee9390", "ee8186"], # [SHORTCAKE] U+E4D0 -> U+E046
  ["ee96b4", "ee8d80"], # [STEAMING BOWL] U+E5B4 -> U+E340
  ["eeaaaf", "ee8cb9"], # [BREAD] U+EAAF -> U+E339
  ["ee9391", "ee8587"], # [COOKING] U+E4D1 -> U+E147
  ["eeaab0", "ee8cba"], # [SOFT ICE CREAM] U+EAB0 -> U+E33A
  ["eeaab1", "ee8cbb"], # [FRENCH FRIES] U+EAB1 -> U+E33B
  ["eeaab2", "ee8cbc"], # [DANGO] U+EAB2 -> U+E33C
  ["eeaab3", "ee8cbd"], # [RICE CRACKER] U+EAB3 -> U+E33D
  ["eeaab4", "ee8cbe"], # [COOKED RICE] U+EAB4 -> U+E33E
  ["eeaab5", "ee8cbf"], # [SPAGHETTI] U+EAB5 -> U+E33F
  ["eeaab6", "ee8d81"], # [CURRY AND RICE] U+EAB6 -> U+E341
  ["eeaab7", "ee8d83"], # [ODEN] U+EAB7 -> U+E343
  ["eeaab8", "ee8d84"], # [SUSHI] U+EAB8 -> U+E344
  ["eeaabd", "ee8d8c"], # [BENTO BOX] U+EABD -> U+E34C
  ["eeaabe", "ee8d8d"], # [POT OF FOOD] U+EABE -> U+E34D
  ["eeabaa", "ee90bf"], # [SHAVED ICE] U+EAEA -> U+E43F
  ["ee9384", :undef], # [MEAT ON BONE] U+E4C4 -> "[肉]"
  ["ee93ad", :undef], # [FISH CAKE WITH SWIRL DESIGN] U+E4ED -> "[なると]"
  ["eeacba", :undef], # [ROASTED SWEET POTATO] U+EB3A -> "[やきいも]"
  ["eeacbb", :undef], # [SLICE OF PIZZA] U+EB3B -> "[ピザ]"
  ["eeacbc", :undef], # [POULTRY LEG] U+EB3C -> "[チキン]"
  ["eead8a", :undef], # [ICE CREAM] U+EB4A -> "[アイスクリーム]"
  ["eead8b", :undef], # [DOUGHNUT] U+EB4B -> "[ドーナツ]"
  ["eead8c", :undef], # [COOKIE] U+EB4C -> "[クッキー]"
  ["eead8d", :undef], # [CHOCOLATE BAR] U+EB4D -> "[チョコ]"
  ["eead8e", :undef], # [CANDY] U+EB4E -> "[キャンディ]"
  ["eead8f", :undef], # [LOLLIPOP] U+EB4F -> "[キャンディ]"
  ["eead96", :undef], # [CUSTARD] U+EB56 -> "[プリン]"
  ["eead99", :undef], # [HONEY POT] U+EB59 -> "[ハチミツ]"
  ["eeadb0", :undef], # [FRIED SHRIMP] U+EB70 -> "[エビフライ]"
  ["ee92ac", "ee8183"], # [FORK AND KNIFE] U+E4AC -> U+E043
  ["ee9697", "ee8185"], # [HOT BEVERAGE] U+E597 -> U+E045
  ["ee9382", "ee8184"], # [COCKTAIL GLASS] U+E4C2 -> U+E044
  ["ee9383", "ee8187"], # [BEER MUG] U+E4C3 -> U+E047
  ["eeaaae", "ee8cb8"], # [TEACUP WITHOUT HANDLE] U+EAAE -> U+E338
  ["eeaa97", "ee8c8b"], # [SAKE BOTTLE AND CUP] U+EA97 -> U+E30B
  ["ee9381", "ee8184"], # [WINE GLASS] U+E4C1 -> U+E044
  ["eeaa98", "ee8c8c"], # [CLINKING BEER MUGS] U+EA98 -> U+E30C
  ["eeacbe", "ee8184"], # [TROPICAL DRINK] U+EB3E -> U+E044
  ["ee9595", "ee88b6"], # [NORTH EAST ARROW] U+E555 -> U+E236
  ["ee958d", "ee88b8"], # [SOUTH EAST ARROW] U+E54D -> U+E238
  ["ee958c", "ee88b7"], # [NORTH WEST ARROW] U+E54C -> U+E237
  ["ee9596", "ee88b9"], # [SOUTH WEST ARROW] U+E556 -> U+E239
  ["eeacad", "ee88b6"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+EB2D -> U+E236
  ["eeacae", "ee88b8"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+EB2E -> U+E238
  ["eeadba", :undef], # [LEFT RIGHT ARROW] U+EB7A -> "⇔"
  ["eeadbb", :undef], # [UP DOWN ARROW] U+EB7B -> "↑↓"
  ["ee94bf", "ee88b2"], # [UPWARDS BLACK ARROW] U+E53F -> U+E232
  ["ee9580", "ee88b3"], # [DOWNWARDS BLACK ARROW] U+E540 -> U+E233
  ["ee9592", "ee88b4"], # [BLACK RIGHTWARDS ARROW] U+E552 -> U+E234
  ["ee9593", "ee88b5"], # [LEFTWARDS BLACK ARROW] U+E553 -> U+E235
  ["ee94ae", "ee88ba"], # [BLACK RIGHT-POINTING TRIANGLE] U+E52E -> U+E23A
  ["ee94ad", "ee88bb"], # [BLACK LEFT-POINTING TRIANGLE] U+E52D -> U+E23B
  ["ee94b0", "ee88bc"], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+E530 -> U+E23C
  ["ee94af", "ee88bd"], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+E52F -> U+E23D
  ["ee9585", :undef], # [BLACK UP-POINTING DOUBLE TRIANGLE] U+E545 -> "▲"
  ["ee9584", :undef], # [BLACK DOWN-POINTING DOUBLE TRIANGLE] U+E544 -> "▼"
  ["ee959a", :undef], # [UP-POINTING RED TRIANGLE] U+E55A -> "▲"
  ["ee959b", :undef], # [DOWN-POINTING RED TRIANGLE] U+E55B -> "▼"
  ["ee9583", :undef], # [UP-POINTING SMALL RED TRIANGLE] U+E543 -> "▲"
  ["ee9582", :undef], # [DOWN-POINTING SMALL RED TRIANGLE] U+E542 -> "▼"
  ["eeaaad", "ee8cb2"], # [HEAVY LARGE CIRCLE] U+EAAD -> U+E332
  ["ee9590", "ee8cb3"], # [CROSS MARK] U+E550 -> U+E333
  ["ee9591", "ee8cb3"], # [NEGATIVE SQUARED CROSS MARK] U+E551 -> U+E333
  ["ee9282", "ee80a1"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E482 -> U+E021
  ["eeacaf", :undef], # [EXCLAMATION QUESTION MARK] U+EB2F -> "！？"
  ["eeacb0", :undef], # [DOUBLE EXCLAMATION MARK] U+EB30 -> "！！"
  ["ee9283", "ee80a0"], # [BLACK QUESTION MARK ORNAMENT] U+E483 -> U+E020
  ["eeacb1", :undef], # [CURLY LOOP] U+EB31 -> "～"
  ["ee9695", "ee80a2"], # [HEAVY BLACK HEART] U+E595 -> U+E022
  ["eeadb5", "ee8ca7"], # [BEATING HEART] U+EB75 -> U+E327
  ["ee91b7", "ee80a3"], # [BROKEN HEART] U+E477 -> U+E023
  ["ee91b8", "ee8ca7"], # [TWO HEARTS] U+E478 -> U+E327
  ["eeaaa6", "ee8ca7"], # [SPARKLING HEART] U+EAA6 -> U+E327
  ["ee93aa", "ee8ca9"], # [HEART WITH ARROW] U+E4EA -> U+E329
  ["eeaaa7", "ee8caa"], # [BLUE HEART] U+EAA7 -> U+E32A
  ["eeaaa8", "ee8cab"], # [GREEN HEART] U+EAA8 -> U+E32B
  ["eeaaa9", "ee8cac"], # [YELLOW HEART] U+EAA9 -> U+E32C
  ["eeaaaa", "ee8cad"], # [PURPLE HEART] U+EAAA -> U+E32D
  ["eead94", "ee90b7"], # [HEART WITH RIBBON] U+EB54 -> U+E437
  ["ee96af", "ee8ca7"], # [REVOLVING HEARTS] U+E5AF -> U+E327
  ["eeaaa5", "ee888c"], # [BLACK HEART SUIT] U+EAA5 -> U+E20C
  ["ee96a1", "ee888e"], # [BLACK SPADE SUIT] U+E5A1 -> U+E20E
  ["ee96a2", "ee888d"], # [BLACK DIAMOND SUIT] U+E5A2 -> U+E20D
  ["ee96a3", "ee888f"], # [BLACK CLUB SUIT] U+E5A3 -> U+E20F
  ["ee91bd", "ee8c8e"], # [SMOKING SYMBOL] U+E47D -> U+E30E
  ["ee91be", "ee8888"], # [NO SMOKING SYMBOL] U+E47E -> U+E208
  ["ee91bf", "ee888a"], # [WHEELCHAIR SYMBOL] U+E47F -> U+E20A
  ["eeacac", :undef], # [TRIANGULAR FLAG ON POST] U+EB2C -> "[旗]"
  ["ee9281", "ee8992"], # [WARNING SIGN] U+E481 -> U+E252
  ["ee9284", "ee84b7"], # [NO ENTRY] U+E484 -> U+E137
  ["eeadb9", :undef], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+EB79 -> "↑↓"
  ["ee92ae", "ee84b6"], # [BICYCLE] U+E4AE -> U+E136
  ["eeadb2", "ee8881"], # [PEDESTRIAN] U+EB72 -> U+E201
  ["ee9798", "ee84bf"], # [BATH] U+E5D8 -> U+E13F
  ["ee92a5", "ee8591"], # [RESTROOM] U+E4A5 -> U+E151
  ["ee9581", :undef], # [NO ENTRY SIGN] U+E541 -> "[禁止]"
  ["ee9597", :undef], # [HEAVY CHECK MARK] U+E557 -> "[チェックマーク]"
  ["ee96ab", :undef], # [SQUARED CL] U+E5AB -> "[CL]"
  ["eeaa85", "ee8894"], # [SQUARED COOL] U+EA85 -> U+E214
  ["ee95b8", :undef], # [SQUARED FREE] U+E578 -> "[FREE]"
  ["eeaa88", "ee88a9"], # [SQUARED ID] U+EA88 -> U+E229
  ["ee96b5", "ee8892"], # [SQUARED NEW] U+E5B5 -> U+E212
  ["ee96ad", "ee898d"], # [SQUARED OK] U+E5AD -> U+E24D
  ["ee93a8", :undef], # [SQUARED SOS] U+E4E8 -> "[SOS]"
  ["ee948f", "ee8893"], # [SQUARED UP WITH EXCLAMATION MARK] U+E50F -> U+E213
  ["ee9792", "ee84ae"], # [SQUARED VS] U+E5D2 -> U+E12E
  ["eeaa87", "ee88a8"], # [SQUARED KATAKANA SA] U+EA87 -> U+E228
  ["eeaa8a", "ee88ab"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+EA8A -> U+E22B
  ["eeaa89", "ee88aa"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+EA89 -> U+E22A
  ["eeaa86", "ee88a7"], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+EA86 -> U+E227
  ["eeaa8b", "ee88ac"], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+EA8B -> U+E22C
  ["eeaa8c", "ee88ad"], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+EA8C -> U+E22D
  ["ee93b1", "ee8c95"], # [CIRCLED IDEOGRAPH SECRET] U+E4F1 -> U+E315
  ["eeaa99", "ee8c8d"], # [CIRCLED IDEOGRAPH CONGRATULATION] U+EA99 -> U+E30D
  ["ee93b7", "ee88a6"], # [CIRCLED IDEOGRAPH ADVANTAGE] U+E4F7 -> U+E226
  ["eeac81", :undef], # [CIRCLED IDEOGRAPH ACCEPT] U+EB01 -> "[可]"
  ["ee94bc", :undef], # [HEAVY PLUS SIGN] U+E53C -> "[＋]"
  ["ee94bd", :undef], # [HEAVY MINUS SIGN] U+E53D -> "[－]"
  ["ee958f", "ee8cb3"], # [HEAVY MULTIPLICATION X] U+E54F -> U+E333
  ["ee9594", :undef], # [HEAVY DIVISION SIGN] U+E554 -> "[÷]"
  ["ee91b6", "ee848f"], # [ELECTRIC LIGHT BULB] U+E476 -> U+E10F
  ["ee93a5", "ee8cb4"], # [ANGER SYMBOL] U+E4E5 -> U+E334
  ["ee91ba", "ee8c91"], # [BOMB] U+E47A -> U+E311
  ["ee91b5", "ee84bc"], # [SLEEPING SYMBOL] U+E475 -> U+E13C
  ["ee96b0", :undef], # [COLLISION SYMBOL] U+E5B0 -> "[ドンッ]"
  ["ee96b1", "ee8cb1"], # [SPLASHING SWEAT SYMBOL] U+E5B1 -> U+E331
  ["ee93a6", "ee8cb1"], # [DROP OF WATER] U+E4E6 -> U+E331
  ["ee93b4", "ee8cb0"], # [DASH SYMBOL] U+E4F4 -> U+E330
  ["ee93b5", "ee819a"], # [PILE OF POO] U+E4F5 -> U+E05A
  ["ee93a9", "ee858c"], # [FLEXED BICEPS] U+E4E9 -> U+E14C
  ["eead9c", "ee9087"], # [DIZZY SYMBOL] U+EB5C -> U+E407
  ["ee93bd", :undef], # [SPEECH BALLOON] U+E4FD -> "[フキダシ]"
  ["eeaaab", "ee8cae"], # [SPARKLES] U+EAAB -> U+E32E
  ["ee91b9", "ee8885"], # [EIGHT POINTED BLACK STAR] U+E479 -> U+E205
  ["ee94be", "ee8886"], # [EIGHT SPOKED ASTERISK] U+E53E -> U+E206
  ["ee94ba", "ee8899"], # [MEDIUM WHITE CIRCLE] U+E53A -> U+E219
  ["ee94bb", "ee8899"], # [MEDIUM BLACK CIRCLE] U+E53B -> U+E219
  ["ee958a", "ee8899"], # [LARGE RED CIRCLE] U+E54A -> U+E219
  ["ee958b", "ee889a"], # [LARGE BLUE CIRCLE] U+E54B -> U+E21A
  ["ee928b", "ee8caf"], # [WHITE MEDIUM STAR] U+E48B -> U+E32F
  ["ee9588", "ee889b"], # [WHITE LARGE SQUARE] U+E548 -> U+E21B
  ["ee9589", "ee889a"], # [BLACK LARGE SQUARE] U+E549 -> U+E21A
  ["ee94b1", "ee889b"], # [WHITE SMALL SQUARE] U+E531 -> U+E21B
  ["ee94b2", "ee889a"], # [BLACK SMALL SQUARE] U+E532 -> U+E21A
  ["ee94b4", "ee889b"], # [WHITE MEDIUM SMALL SQUARE] U+E534 -> U+E21B
  ["ee94b5", "ee889a"], # [BLACK MEDIUM SMALL SQUARE] U+E535 -> U+E21A
  ["ee94b8", "ee889b"], # [WHITE MEDIUM SQUARE] U+E538 -> U+E21B
  ["ee94b9", "ee889a"], # [BLACK MEDIUM SQUARE] U+E539 -> U+E21A
  ["ee9586", "ee889b"], # [LARGE ORANGE DIAMOND] U+E546 -> U+E21B
  ["ee9587", "ee889b"], # [LARGE BLUE DIAMOND] U+E547 -> U+E21B
  ["ee94b6", "ee889b"], # [SMALL ORANGE DIAMOND] U+E536 -> U+E21B
  ["ee94b7", "ee889b"], # [SMALL BLUE DIAMOND] U+E537 -> U+E21B
  ["ee91ac", "ee8cae"], # [SPARKLE] U+E46C -> U+E32E
  ["ee93b0", :undef], # [WHITE FLOWER] U+E4F0 -> "[花丸]"
  ["ee93b2", :undef], # [HUNDRED POINTS SYMBOL] U+E4F2 -> "[100点]"
  ["ee959d", :undef], # [LEFTWARDS ARROW WITH HOOK] U+E55D -> "←┘"
  ["ee959c", :undef], # [RIGHTWARDS ARROW WITH HOOK] U+E55C -> "└→"
  ["eeac8d", :undef], # [CLOCKWISE DOWNWARDS AND UPWARDS OPEN CIRCLE ARROWS] U+EB0D -> "↑↓"
  ["ee9491", "ee8581"], # [SPEAKER WITH THREE SOUND WAVES] U+E511 -> U+E141
  ["ee9684", :undef], # [BATTERY] U+E584 -> "[電池]"
  ["ee9689", :undef], # [ELECTRIC PLUG] U+E589 -> "[コンセント]"
  ["ee9498", "ee8494"], # [LEFT-POINTING MAGNIFYING GLASS] U+E518 -> U+E114
  ["eeac85", "ee8494"], # [RIGHT-POINTING MAGNIFYING GLASS] U+EB05 -> U+E114
  ["ee949c", "ee8584"], # [LOCK] U+E51C -> U+E144
  ["eeac8c", "ee8584"], # [LOCK WITH INK PEN] U+EB0C -> U+E144
  ["eeabbc", "ee8584"], # [CLOSED LOCK WITH KEY] U+EAFC -> U+E144
  ["ee9499", "ee80bf"], # [KEY] U+E519 -> U+E03F
  ["ee9492", "ee8ca5"], # [BELL] U+E512 -> U+E325
  ["eeac82", :undef], # [BALLOT BOX WITH CHECK] U+EB02 -> "[チェックマーク]"
  ["eeac84", :undef], # [RADIO BUTTON] U+EB04 -> "[ラジオボタン]"
  ["eeac87", :undef], # [BOOKMARK] U+EB07 -> "[ブックマーク]"
  ["ee968a", :undef], # [LINK SYMBOL] U+E58A -> "[リンク]"
  ["eeac86", "ee88b5"], # [BACK WITH LEFTWARDS ARROW ABOVE] U+EB06 -> U+E235
  ["ee968c", :undef], # [EM SPACE] U+E58C -> U+3013 (GETA)
  ["ee968d", :undef], # [EN SPACE] U+E58D -> U+3013 (GETA)
  ["ee968e", :undef], # [FOUR-PER-EM SPACE] U+E58E -> U+3013 (GETA)
  ["ee959e", :undef], # [WHITE HEAVY CHECK MARK] U+E55E -> "[チェックマーク]"
  ["eeae83", "ee8090"], # [RAISED FIST] U+EB83 -> U+E010
  ["ee96a7", "ee8092"], # [RAISED HAND] U+E5A7 -> U+E012
  ["ee96a6", "ee8091"], # [VICTORY HAND] U+E5A6 -> U+E011
  ["ee93b3", "ee808d"], # [FISTED HAND SIGN] U+E4F3 -> U+E00D
  ["ee93b9", "ee808e"], # [THUMBS UP SIGN] U+E4F9 -> U+E00E
  ["ee93b6", "ee808f"], # [WHITE UP POINTING INDEX] U+E4F6 -> U+E00F
  ["eeaa8d", "ee88ae"], # [WHITE UP POINTING BACKHAND INDEX] U+EA8D -> U+E22E
  ["eeaa8e", "ee88af"], # [WHITE DOWN POINTING BACKHAND INDEX] U+EA8E -> U+E22F
  ["ee93bf", "ee88b0"], # [WHITE LEFT POINTING BACKHAND INDEX] U+E4FF -> U+E230
  ["ee9480", "ee88b1"], # [WHITE RIGHT POINTING BACKHAND INDEX] U+E500 -> U+E231
  ["eeab96", "ee909e"], # [WAVING HAND SIGN] U+EAD6 -> U+E41E
  ["eeab93", "ee909f"], # [CLAPPING HANDS SIGN] U+EAD3 -> U+E41F
  ["eeab94", "ee90a0"], # [OK HAND SIGN] U+EAD4 -> U+E420
  ["eeab95", "ee90a1"], # [THUMBS DOWN SIGN] U+EAD5 -> U+E421
  ["ee95b7", :undef], # [EMOJI COMPATIBILITY SYMBOL-37] U+E577 -> "[EZ]"
  ["ee96b2", :undef], # [EMOJI COMPATIBILITY SYMBOL-38] U+E5B2 -> "[ezplus]"
  ["eeaa9d", :undef], # [EMOJI COMPATIBILITY SYMBOL-39] U+EA9D -> "[EZナビ]"
  ["eeadb4", :undef], # [EMOJI COMPATIBILITY SYMBOL-40] U+EB74 -> "[EZムービー]"
  ["eeae81", :undef], # [EMOJI COMPATIBILITY SYMBOL-41] U+EB81 -> "[Cメール]"
  ["eeae89", :undef], # [EMOJI COMPATIBILITY SYMBOL-42] U+EB89 -> "[Java]"
  ["eeae8a", :undef], # [EMOJI COMPATIBILITY SYMBOL-43] U+EB8A -> "[BREW]"
  ["eeae8b", :undef], # [EMOJI COMPATIBILITY SYMBOL-44] U+EB8B -> "[EZ着うた]"
  ["eeae8c", :undef], # [EMOJI COMPATIBILITY SYMBOL-45] U+EB8C -> "[EZナビ]"
  ["eeae8d", :undef], # [EMOJI COMPATIBILITY SYMBOL-46] U+EB8D -> "[WIN]"
  ["eeae8e", :undef], # [EMOJI COMPATIBILITY SYMBOL-47] U+EB8E -> "[プレミアム]"
  ["eeabbb", :undef], # [EMOJI COMPATIBILITY SYMBOL-48] U+EAFB -> "[オープンウェブ]"
  ["ee9686", :undef], # [EMOJI COMPATIBILITY SYMBOL-49] U+E586 -> "[PDC]"
  ["ee94ac", :undef], # [EMOJI COMPATIBILITY SYMBOL-66] U+E52C -> "[Q]"
  # for undocumented codepoints
  ["eebda0", "ee818a"], # [BLACK SUN WITH RAYS] U+E488 -> U+E04A
  ["eebda5", "ee8189"], # [CLOUD] U+E48D -> U+E049
  ["eebda4", "ee818b"], # [UMBRELLA WITH RAIN DROPS] U+E48C -> U+E04B
  ["eebd9d", "ee8188"], # [SNOWMAN WITHOUT SNOW] U+E485 -> U+E048
  ["eebd9f", "ee84bd"], # [HIGH VOLTAGE SIGN] U+E487 -> U+E13D
  ["eebd81", "ee9183"], # [CYCLONE] U+E469 -> U+E443
  ["ef82b5", :undef], # [FOGGY] U+E598 -> "[霧]"
  ["eeb2bc", "ee90bc"], # [CLOSED UMBRELLA] U+EAE8 -> U+E43C
  ["eeb385", "ee918b"], # [NIGHT WITH STARS] U+EAF1 -> U+E44B
  ["eeb388", "ee9189"], # [SUNRISE] U+EAF4 -> U+E449
  ["eeb18d", "ee8586"], # [CITYSCAPE AT DUSK] U+E5DA -> U+E146
  ["eeb386", "ee918c"], # [RAINBOW] U+EAF2 -> U+E44C
  ["eebda2", :undef], # [SNOWFLAKE] U+E48A -> "[雪結晶]"
  ["eebda6", "ee818aee8189"], # [SUN BEHIND CLOUD] U+E48E -> U+E04A U+E049
  ["eebe98", "ee918b"], # [BRIDGE AT NIGHT] U+E4BF -> U+E44B
  ["eeb681", "ee90be"], # [WATER WAVE] U+EB7C -> U+E43E
  ["eeb597", :undef], # [VOLCANO] U+EB53 -> "[火山]"
  ["eeb5a3", "ee918b"], # [MILKY WAY] U+EB5F -> U+E44B
  ["ef8390", :undef], # [EARTH GLOBE ASIA-AUSTRALIA] U+E5B3 -> "[地球]"
  ["ef8385", :undef], # [NEW MOON SYMBOL] U+E5A8 -> "●"
  ["ef8386", "ee818c"], # [WAXING GIBBOUS MOON SYMBOL] U+E5A9 -> U+E04C
  ["ef8387", "ee818c"], # [FIRST QUARTER MOON SYMBOL] U+E5AA -> U+E04C
  ["eebd9e", "ee818c"], # [CRESCENT MOON] U+E486 -> U+E04C
  ["eebda1", "ee818c"], # [FIRST QUARTER MOON WITH FACE] U+E489 -> U+E04C
  ["eebd80", :undef], # [SHOOTING STAR] U+E468 -> "☆彡"
  ["ef8297", :undef], # [WATCH] U+E57A -> "[腕時計]"
  ["ef8298", :undef], # [HOURGLASS] U+E57B -> "[砂時計]"
  ["ef82b1", "ee80ad"], # [ALARM CLOCK] U+E594 -> U+E02D
  ["eebd94", :undef], # [HOURGLASS WITH FLOWING SAND] U+E47C -> "[砂時計]"
  ["eebda7", "ee88bf"], # [ARIES] U+E48F -> U+E23F
  ["eebda8", "ee8980"], # [TAURUS] U+E490 -> U+E240
  ["eebda9", "ee8981"], # [GEMINI] U+E491 -> U+E241
  ["eebdaa", "ee8982"], # [CANCER] U+E492 -> U+E242
  ["eebdab", "ee8983"], # [LEO] U+E493 -> U+E243
  ["eebdac", "ee8984"], # [VIRGO] U+E494 -> U+E244
  ["eebdad", "ee8985"], # [LIBRA] U+E495 -> U+E245
  ["eebdae", "ee8986"], # [SCORPIUS] U+E496 -> U+E246
  ["eebdaf", "ee8987"], # [SAGITTARIUS] U+E497 -> U+E247
  ["eebdb0", "ee8988"], # [CAPRICORN] U+E498 -> U+E248
  ["eebdb1", "ee8989"], # [AQUARIUS] U+E499 -> U+E249
  ["eebdb2", "ee898a"], # [PISCES] U+E49A -> U+E24A
  ["eebdb3", "ee898b"], # [OPHIUCHUS] U+E49B -> U+E24B
  ["eebfac", "ee8490"], # [FOUR LEAF CLOVER] U+E513 -> U+E110
  ["eebebd", "ee8c84"], # [TULIP] U+E4E4 -> U+E304
  ["eeb682", "ee8490"], # [SEEDLING] U+EB7D -> U+E110
  ["eebea7", "ee8498"], # [MAPLE LEAF] U+E4CE -> U+E118
  ["eebea3", "ee80b0"], # [CHERRY BLOSSOM] U+E4CA -> U+E030
  ["ef83aa", "ee80b2"], # [ROSE] U+E5BA -> U+E032
  ["eeb180", "ee8499"], # [FALLEN LEAF] U+E5CD -> U+E119
  ["eeb1a7", "ee8c83"], # [HIBISCUS] U+EA94 -> U+E303
  ["eebebc", "ee8c85"], # [SUNFLOWER] U+E4E3 -> U+E305
  ["eebebb", "ee8c87"], # [PALM TREE] U+E4E2 -> U+E307
  ["eeb1a9", "ee8c88"], # [CACTUS] U+EA96 -> U+E308
  ["eeb3b7", :undef], # [EAR OF MAIZE] U+EB36 -> "[とうもろこし]"
  ["eeb3b8", :undef], # [MUSHROOM] U+EB37 -> "[キノコ]"
  ["eeb3b9", :undef], # [CHESTNUT] U+EB38 -> "[栗]"
  ["eeb58d", "ee8c85"], # [BLOSSOM] U+EB49 -> U+E305
  ["eeb687", "ee8490"], # [HERB] U+EB82 -> U+E110
  ["eebeab", :undef], # [CHERRIES] U+E4D2 -> "[さくらんぼ]"
  ["eeb3b6", :undef], # [BANANA] U+EB35 -> "[バナナ]"
  ["eeb28d", "ee8d85"], # [RED APPLE] U+EAB9 -> U+E345
  ["eeb28e", "ee8d86"], # [TANGERINE] U+EABA -> U+E346
  ["eebead", "ee8d87"], # [STRAWBERRY] U+E4D4 -> U+E347
  ["eebea6", "ee8d88"], # [WATERMELON] U+E4CD -> U+E348
  ["eeb28f", "ee8d89"], # [TOMATO] U+EABB -> U+E349
  ["eeb290", "ee8d8a"], # [AUBERGINE] U+EABC -> U+E34A
  ["eeb3b3", :undef], # [MELON] U+EB32 -> "[メロン]"
  ["eeb3b4", :undef], # [PINEAPPLE] U+EB33 -> "[パイナップル]"
  ["eeb3b5", :undef], # [GRAPES] U+EB34 -> "[ブドウ]"
  ["eeb3ba", :undef], # [PEACH] U+EB39 -> "[モモ]"
  ["eeb59e", "ee8d85"], # [GREEN APPLE] U+EB5A -> U+E345
  ["ef8381", "ee9099"], # [EYES] U+E5A4 -> U+E419
  ["ef8382", "ee909b"], # [EAR] U+E5A5 -> U+E41B
  ["eeb2a4", "ee909a"], # [NOSE] U+EAD0 -> U+E41A
  ["eeb2a5", "ee909c"], # [MOUTH] U+EAD1 -> U+E41C
  ["eeb58b", "ee9089"], # [TONGUE] U+EB47 -> U+E409
  ["eebfa2", "ee8c9c"], # [LIPSTICK] U+E509 -> U+E31C
  ["eeb1b3", "ee8c9d"], # [NAIL POLISH] U+EAA0 -> U+E31D
  ["eebfa4", "ee8c9e"], # [FACE MASSAGE] U+E50B -> U+E31E
  ["eeb1b4", "ee8c9f"], # [HAIRCUT] U+EAA1 -> U+E31F
  ["eeb1b5", "ee8ca0"], # [BARBER POLE] U+EAA2 -> U+E320
  ["eebf95", "ee8081"], # [BOY] U+E4FC -> U+E001
  ["eebf93", "ee8082"], # [GIRL] U+E4FA -> U+E002
  ["eebf9a", :undef], # [FAMILY] U+E501 -> "[家族]"
  ["eeb190", "ee8592"], # [POLICE OFFICER] U+E5DD -> U+E152
  ["eeb2af", "ee90a9"], # [WOMAN WITH BUNNY EARS] U+EADB -> U+E429
  ["eeb2bd", :undef], # [BRIDE WITH VEIL] U+EAE9 -> "[花嫁]"
  ["eeb394", "ee9495"], # [WESTERN PERSON] U+EB13 -> U+E515
  ["eeb395", "ee9496"], # [MAN WITH GUA PI MAO] U+EB14 -> U+E516
  ["eeb396", "ee9497"], # [MAN WITH TURBAN] U+EB15 -> U+E517
  ["eeb397", "ee9498"], # [OLDER MAN] U+EB16 -> U+E518
  ["eeb398", "ee9499"], # [OLDER WOMAN] U+EB17 -> U+E519
  ["eeb399", "ee949a"], # [BABY] U+EB18 -> U+E51A
  ["eeb39a", "ee949b"], # [CONSTRUCTION WORKER] U+EB19 -> U+E51B
  ["eeb39b", "ee949c"], # [PRINCESS] U+EB1A -> U+E51C
  ["eeb588", :undef], # [JAPANESE OGRE] U+EB44 -> "[なまはげ]"
  ["eeb589", :undef], # [JAPANESE GOBLIN] U+EB45 -> "[天狗]"
  ["eebea4", "ee849b"], # [GHOST] U+E4CB -> U+E11B
  ["ef83af", "ee818e"], # [BABY ANGEL] U+E5BF -> U+E04E
  ["eebfa7", "ee848c"], # [EXTRATERRESTRIAL ALIEN] U+E50E -> U+E10C
  ["eebf85", "ee84ab"], # [ALIEN MONSTER] U+E4EC -> U+E12B
  ["eebf88", "ee849a"], # [IMP] U+E4EF -> U+E11A
  ["eebf91", "ee849c"], # [SKULL] U+E4F8 -> U+E11C
  ["eeb39d", "ee949f"], # [DANCER] U+EB1C -> U+E51F
  ["eeb683", :undef], # [SNAIL] U+EB7E -> "[カタツムリ]"
  ["eeb3a3", "ee94ad"], # [SNAKE] U+EB22 -> U+E52D
  ["eeb3a4", "ee94ae"], # [CHICKEN] U+EB23 -> U+E52E
  ["eeb3a5", "ee94af"], # [BOAR] U+EB24 -> U+E52F
  ["eeb3a6", "ee94b0"], # [BACTRIAN CAMEL] U+EB25 -> U+E530
  ["eeb3a0", "ee94a6"], # [ELEPHANT] U+EB1F -> U+E526
  ["eeb3a1", "ee94a7"], # [KOALA] U+EB20 -> U+E527
  ["ef83b7", "ee848a"], # [OCTOPUS] U+E5C7 -> U+E10A
  ["eeb380", "ee9181"], # [SPIRAL SHELL] U+EAEC -> U+E441
  ["eeb39f", "ee94a5"], # [BUG] U+EB1E -> U+E525
  ["eebeb6", :undef], # [ANT] U+E4DD -> "[アリ]"
  ["eeb59b", :undef], # [HONEYBEE] U+EB57 -> "[ミツバチ]"
  ["eeb59c", :undef], # [LADY BEETLE] U+EB58 -> "[てんとう虫]"
  ["eeb39e", "ee94a2"], # [TROPICAL FISH] U+EB1D -> U+E522
  ["eebeac", "ee8099"], # [BLOWFISH] U+E4D3 -> U+E019
  ["eeb187", :undef], # [TURTLE] U+E5D4 -> "[カメ]"
  ["eebeb9", "ee94a3"], # [BABY CHICK] U+E4E0 -> U+E523
  ["eeb5ba", "ee94a3"], # [FRONT-FACING BABY CHICK] U+EB76 -> U+E523
  ["eeb18e", "ee94a3"], # [HATCHING CHICK] U+E5DB -> U+E523
  ["eebeb5", "ee8195"], # [PENGUIN] U+E4DC -> U+E055
  ["eebeb8", "ee8192"], # [POODLE] U+E4DF -> U+E052
  ["eeb39c", "ee94a0"], # [DOLPHIN] U+EB1B -> U+E520
  ["ef83b2", "ee8193"], # [MOUSE FACE] U+E5C2 -> U+E053
  ["ef83b0", "ee8190"], # [TIGER FACE] U+E5C0 -> U+E050
  ["eebeb4", "ee818f"], # [CAT FACE] U+E4DB -> U+E04F
  ["eebd88", "ee8194"], # [SPOUTING WHALE] U+E470 -> U+E054
  ["eebeb1", "ee809a"], # [HORSE FACE] U+E4D8 -> U+E01A
  ["eebeb2", "ee8489"], # [MONKEY FACE] U+E4D9 -> U+E109
  ["eebeba", "ee8192"], # [DOG FACE] U+E4E1 -> U+E052
  ["eebeb7", "ee848b"], # [PIG FACE] U+E4DE -> U+E10B
  ["ef83b1", "ee8191"], # [BEAR FACE] U+E5C1 -> U+E051
  ["eeb3a2", "ee94ab"], # [COW FACE] U+EB21 -> U+E52B
  ["eebeb0", "ee94ac"], # [RABBIT FACE] U+E4D7 -> U+E52C
  ["eebeb3", "ee94b1"], # [FROG FACE] U+E4DA -> U+E531
  ["eebf87", "ee94b6"], # [PAW PRINTS] U+E4EE -> U+E536
  ["eeb583", :undef], # [DRAGON FACE] U+EB3F -> "[辰]"
  ["eeb58a", :undef], # [PANDA FACE] U+EB46 -> "[パンダ]"
  ["eeb58c", "ee848b"], # [PIG NOSE] U+EB48 -> U+E10B
  ["eebd8a", "ee8199"], # [ANGRY FACE] U+E472 -> U+E059
  ["eeb5ab", "ee9083"], # [ANGUISHED FACE] U+EB67 -> U+E403
  ["eeb29e", "ee9090"], # [ASTONISHED FACE] U+EACA -> U+E410
  ["ef838b", "ee9086"], # [DIZZY FACE] U+E5AE -> U+E406
  ["eeb29f", "ee908f"], # [EXASPERATED FACE] U+EACB -> U+E40F
  ["eeb29d", "ee908e"], # [EXPRESSIONLESS FACE] U+EAC9 -> U+E40E
  ["ef83b4", "ee8486"], # [FACE WITH HEART-SHAPED EYES] U+E5C4 -> U+E106
  ["eeb295", "ee9084"], # [FACE WITH LOOK OF TRIUMPH] U+EAC1 -> U+E404
  ["eebf80", "ee8485"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E4E7 -> U+E105
  ["eeb2a3", "ee9098"], # [FACE THROWING A KISS] U+EACF -> U+E418
  ["eeb2a2", "ee9097"], # [FACE KISSING] U+EACE -> U+E417
  ["eeb29b", "ee908c"], # [FACE WITH MASK] U+EAC7 -> U+E40C
  ["eeb29c", "ee908d"], # [FLUSHED FACE] U+EAC8 -> U+E40D
  ["eebd89", "ee8197"], # [HAPPY FACE WITH OPEN MOUTH] U+E471 -> U+E057
  ["eeb685", "ee9084"], # [HAPPY FACE WITH GRIN] U+EB80 -> U+E404
  ["eeb5a8", "ee9092"], # [HAPPY AND CRYING FACE] U+EB64 -> U+E412
  ["eeb2a1", "ee8196"], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+EACD -> U+E056
  ["eebf94", "ee9094"], # [WHITE SMILING FACE] U+E4FB -> U+E414
  ["eeb5ad", "ee9093"], # [CRYING FACE] U+EB69 -> U+E413
  ["eebd8b", "ee9091"], # [LOUDLY CRYING FACE] U+E473 -> U+E411
  ["eeb29a", "ee908b"], # [FEARFUL FACE] U+EAC6 -> U+E40B
  ["eeb296", "ee9086"], # [PERSEVERING FACE] U+EAC2 -> U+E406
  ["eeb5a1", "ee9096"], # [POUTING FACE] U+EB5D -> U+E416
  ["eeb299", "ee908a"], # [RELIEVED FACE] U+EAC5 -> U+E40A
  ["eeb297", "ee9087"], # [CONFOUNDED FACE] U+EAC3 -> U+E407
  ["eeb294", "ee9083"], # [PENSIVE FACE] U+EAC0 -> U+E403
  ["ef83b5", "ee8487"], # [FACE SCREAMING IN FEAR] U+E5C5 -> U+E107
  ["eeb298", "ee9088"], # [SLEEPY FACE] U+EAC4 -> U+E408
  ["eeb293", "ee9082"], # [SMIRKING FACE] U+EABF -> U+E402
  ["ef83b6", "ee8488"], # [FACE WITH COLD SWEAT] U+E5C6 -> U+E108
  ["eebd8c", "ee9086"], # [TIRED FACE] U+E474 -> U+E406
  ["ef83b3", "ee9085"], # [WINKING FACE] U+E5C3 -> U+E405
  ["eeb5a5", "ee8197"], # [CAT FACE WITH OPEN MOUTH] U+EB61 -> U+E057
  ["eeb684", "ee9084"], # [HAPPY CAT FACE WITH GRIN] U+EB7F -> U+E404
  ["eeb5a7", "ee9092"], # [HAPPY AND CRYING CAT FACE] U+EB63 -> U+E412
  ["eeb5a4", "ee9098"], # [CAT FACE KISSING] U+EB60 -> U+E418
  ["eeb5a9", "ee8486"], # [CAT FACE WITH HEART-SHAPED EYES] U+EB65 -> U+E106
  ["eeb5ac", "ee9093"], # [CRYING CAT FACE] U+EB68 -> U+E413
  ["eeb5a2", "ee9096"], # [POUTING CAT FACE] U+EB5E -> U+E416
  ["eeb5ae", "ee9084"], # [CAT FACE WITH TIGHTLY-CLOSED LIPS] U+EB6A -> U+E404
  ["eeb5aa", "ee9083"], # [ANGUISHED CAT FACE] U+EB66 -> U+E403
  ["eeb2ab", "ee90a3"], # [FACE WITH NO GOOD GESTURE] U+EAD7 -> U+E423
  ["eeb2ac", "ee90a4"], # [FACE WITH OK GESTURE] U+EAD8 -> U+E424
  ["eeb2ad", "ee90a6"], # [PERSON BOWING DEEPLY] U+EAD9 -> U+E426
  ["eeb594", :undef], # [SEE-NO-EVIL MONKEY] U+EB50 -> "(/_＼)"
  ["eeb595", :undef], # [SPEAK-NO-EVIL MONKEY] U+EB51 -> "(・×・)"
  ["eeb596", :undef], # [HEAR-NO-EVIL MONKEY] U+EB52 -> "|(・×・)|"
  ["eeb68a", "ee8092"], # [PERSON RAISING ONE HAND] U+EB85 -> U+E012
  ["eeb68b", "ee90a7"], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+EB86 -> U+E427
  ["eeb68c", "ee9083"], # [PERSON FROWNING] U+EB87 -> U+E403
  ["eeb68d", "ee9096"], # [PERSON WITH POUTING FACE] U+EB88 -> U+E416
  ["eeb2a6", "ee909d"], # [PERSON WITH FOLDED HANDS] U+EAD2 -> U+E41D
  ["eebe84", "ee80b6"], # [HOUSE BUILDING] U+E4AB -> U+E036
  ["ef83a0", "ee80b6"], # [HOUSE WITH GARDEN] U+EB09 -> U+E036
  ["eebe86", "ee80b8"], # [OFFICE BUILDING] U+E4AD -> U+E038
  ["eeb191", "ee8593"], # [JAPANESE POST OFFICE] U+E5DE -> U+E153
  ["eeb192", "ee8595"], # [HOSPITAL] U+E5DF -> U+E155
  ["eebe83", "ee858d"], # [BANK] U+E4AA -> U+E14D
  ["eebdbb", "ee8594"], # [AUTOMATED TELLER MACHINE] U+E4A3 -> U+E154
  ["eeb194", "ee8598"], # [HOTEL] U+EA81 -> U+E158
  ["eeb387", "ee9481"], # [LOVE HOTEL] U+EAF3 -> U+E501
  ["eebdbc", "ee8596"], # [CONVENIENCE STORE] U+E4A4 -> U+E156
  ["eeb193", "ee8597"], # [SCHOOL] U+EA80 -> U+E157
  ["ef83ab", "ee80b7"], # [CHURCH] U+E5BB -> U+E037
  ["eeb182", "ee84a1"], # [FOUNTAIN] U+E5CF -> U+E121
  ["eeb38a", "ee9484"], # [DEPARTMENT STORE] U+EAF6 -> U+E504
  ["eeb38b", "ee9485"], # [JAPANESE CASTLE] U+EAF7 -> U+E505
  ["eeb38c", "ee9486"], # [EUROPEAN CASTLE] U+EAF8 -> U+E506
  ["eeb38d", "ee9488"], # [FACTORY] U+EAF9 -> U+E508
  ["eebe82", "ee8882"], # [ANCHOR] U+E4A9 -> U+E202
  ["eebe96", "ee8c8b"], # [IZAKAYA LANTERN] U+E4BD -> U+E30B
  ["ef83ad", "ee80bb"], # [MOUNT FUJI] U+E5BD -> U+E03B
  ["eebe99", "ee9489"], # [TOKYO TOWER] U+E4C0 -> U+E509
  ["ef828f", :undef], # [SILHOUETTE OF JAPAN] U+E572 -> "[日本地図]"
  ["eeb5b0", :undef], # [MOYAI] U+EB6C -> "[モアイ]"
  ["ef83a7", "ee8087"], # [MANS SHOE] U+E5B7 -> U+E007
  ["eeb3ac", "ee8087"], # [ATHLETIC SHOE] U+EB2B -> U+E007
  ["eebfb3", "ee84be"], # [HIGH-HEELED SHOE] U+E51A -> U+E13E
  ["eeb1b2", "ee8c9b"], # [WOMANS BOOTS] U+EA9F -> U+E31B
  ["eeb3ab", "ee94b6"], # [FOOTPRINTS] U+EB2A -> U+E536
  ["eebf97", :undef], # [EYEGLASSES] U+E4FE -> "[メガネ]"
  ["ef83a6", "ee8086"], # [T-SHIRT] U+E5B6 -> U+E006
  ["eeb5bb", :undef], # [JEANS] U+EB77 -> "[ジーンズ]"
  ["ef83b9", "ee848e"], # [CROWN] U+E5C9 -> U+E10E
  ["eeb1a6", "ee8c82"], # [NECKTIE] U+EA93 -> U+E302
  ["eeb1b1", "ee8c98"], # [WOMANS HAT] U+EA9E -> U+E318
  ["eeb5af", "ee8c99"], # [DRESS] U+EB6B -> U+E319
  ["eeb1b6", "ee8ca1"], # [KIMONO] U+EAA3 -> U+E321
  ["eeb1b7", "ee8ca2"], # [BIKINI] U+EAA4 -> U+E322
  ["eebfa6", "ee8086"], # [WOMANS CLOTHES] U+E50D -> U+E006
  ["eebf9d", :undef], # [PURSE] U+E504 -> "[財布]"
  ["eebdb4", "ee8ca3"], # [HANDBAG] U+E49C -> U+E323
  ["eebea0", "ee84af"], # [MONEY BAG] U+E4C7 -> U+E12F
  ["eeb18f", "ee858a"], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+E5DC -> U+E14A
  ["ef8296", "ee84af"], # [HEAVY DOLLAR SIGN] U+E579 -> U+E12F
  ["ef8299", :undef], # [CREDIT CARD] U+E57C -> "[カード]"
  ["ef829a", :undef], # [BANKNOTE WITH YEN SIGN] U+E57D -> "￥"
  ["ef82a2", "ee84af"], # [BANKNOTE WITH DOLLAR SIGN] U+E585 -> U+E12F
  ["eeb59f", :undef], # [MONEY WITH WINGS] U+EB5B -> "[飛んでいくお金]"
  ["eeb392", "ee9493"], # [REGIONAL INDICATOR SYMBOL LETTERS CN] U+EB11 -> U+E513
  ["eeb38f", "ee948e"], # [REGIONAL INDICATOR SYMBOL LETTERS DE] U+EB0E -> U+E50E
  ["eeb188", "ee9491"], # [REGIONAL INDICATOR SYMBOL LETTERS ES] U+E5D5 -> U+E511
  ["eeb38e", "ee948d"], # [REGIONAL INDICATOR SYMBOL LETTERS FR] U+EAFA -> U+E50D
  ["eeb391", "ee9490"], # [REGIONAL INDICATOR SYMBOL LETTERS GB] U+EB10 -> U+E510
  ["eeb390", "ee948f"], # [REGIONAL INDICATOR SYMBOL LETTERS IT] U+EB0F -> U+E50F
  ["eebea5", "ee948b"], # [REGIONAL INDICATOR SYMBOL LETTERS JP] U+E4CC -> U+E50B
  ["eeb393", "ee9494"], # [REGIONAL INDICATOR SYMBOL LETTERS KR] U+EB12 -> U+E514
  ["eeb189", "ee9492"], # [REGIONAL INDICATOR SYMBOL LETTERS RU] U+E5D6 -> U+E512
  ["ef8290", "ee948c"], # [REGIONAL INDICATOR SYMBOL LETTERS US] U+E573 -> U+E50C
  ["eebd93", "ee849d"], # [FIRE] U+E47B -> U+E11D
  ["ef82a0", :undef], # [ELECTRIC TORCH] U+E583 -> "[懐中電灯]"
  ["ef82a4", :undef], # [WRENCH] U+E587 -> "[レンチ]"
  ["ef83bb", "ee8496"], # [HAMMER] U+E5CB -> U+E116
  ["ef829e", :undef], # [NUT AND BOLT] U+E581 -> "[ネジ]"
  ["ef829c", :undef], # [HOCHO] U+E57F -> "[包丁]"
  ["eebfa3", "ee8493"], # [PISTOL] U+E50A -> U+E113
  ["eeb1a2", "ee88be"], # [CRYSTAL BALL] U+EA8F -> U+E23E
  ["eebd98", "ee8889"], # [JAPANESE SYMBOL FOR BEGINNER] U+E480 -> U+E209
  ["eebfa9", "ee84bb"], # [SYRINGE] U+E510 -> U+E13B
  ["eeb1ad", "ee8c8f"], # [PILL] U+EA9A -> U+E30F
  ["eeb3a7", "ee94b2"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+EB26 -> U+E532
  ["eeb3a8", "ee94b3"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+EB27 -> U+E533
  ["eeb3aa", "ee94b4"], # [NEGATIVE SQUARED AB] U+EB29 -> U+E534
  ["eeb3a9", "ee94b5"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+EB28 -> U+E535
  ["ef82bc", "ee8c94"], # [RIBBON] U+E59F -> U+E314
  ["eebea8", "ee8492"], # [WRAPPED PRESENT] U+E4CF -> U+E112
  ["ef82bd", "ee8d8b"], # [BIRTHDAY CAKE] U+E5A0 -> U+E34B
  ["eebea2", "ee80b3"], # [CHRISTMAS TREE] U+E4C9 -> U+E033
  ["eeb384", "ee9188"], # [FATHER CHRISTMAS] U+EAF0 -> U+E448
  ["eeb18c", "ee8583"], # [CROSSED FLAGS] U+E5D9 -> U+E143
  ["ef83bc", "ee8497"], # [FIREWORKS] U+E5CC -> U+E117
  ["eeb1ae", "ee8c90"], # [BALLOON] U+EA9B -> U+E310
  ["eeb1af", "ee8c92"], # [PARTY POPPER] U+EA9C -> U+E312
  ["eeb2b7", "ee90b6"], # [PINE DECORATION] U+EAE3 -> U+E436
  ["eeb2b8", "ee90b8"], # [JAPANESE DOLLS] U+EAE4 -> U+E438
  ["eeb2b9", "ee90b9"], # [GRADUATION CAP] U+EAE5 -> U+E439
  ["eeb2ba", "ee90ba"], # [SCHOOL SATCHEL] U+EAE6 -> U+E43A
  ["eeb2bb", "ee90bb"], # [CARP STREAMER] U+EAE7 -> U+E43B
  ["eeb2bf", "ee9180"], # [FIREWORK SPARKLER] U+EAEB -> U+E440
  ["eeb381", "ee9182"], # [WIND CHIME] U+EAED -> U+E442
  ["eeb382", "ee9185"], # [JACK-O-LANTERN] U+EAEE -> U+E445
  ["eebd87", :undef], # [CONFETTI BALL] U+E46F -> "[オメデトウ]"
  ["eeb581", :undef], # [TANABATA TREE] U+EB3D -> "[七夕]"
  ["eeb383", "ee9186"], # [MOON VIEWING CEREMONY] U+EAEF -> U+E446
  ["ef82b8", :undef], # [PAGER] U+E59B -> "[ポケベル]"
  ["ef82b3", "ee8089"], # [BLACK TELEPHONE] U+E596 -> U+E009
  ["eebfb7", "ee8089"], # [TELEPHONE RECEIVER] U+E51E -> U+E009
  ["ef82a5", "ee808a"], # [MOBILE PHONE] U+E588 -> U+E00A
  ["ef839f", "ee8484"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+EB08 -> U+E104
  ["eeb1a5", "ee8c81"], # [MEMO] U+EA92 -> U+E301
  ["eebfb9", "ee808b"], # [FAX MACHINE] U+E520 -> U+E00B
  ["eebfba", "ee8483"], # [ENVELOPE] U+E521 -> U+E103
  ["ef82ae", "ee8483"], # [INCOMING ENVELOPE] U+E591 -> U+E103
  ["eeb5a6", "ee8483"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+EB62 -> U+E103
  ["eebfb4", "ee8481"], # [CLOSED MAILBOX WITH LOWERED FLAG] U+E51B -> U+E101
  ["ef83a1", "ee8481"], # [CLOSED MAILBOX WITH RAISED FLAG] U+EB0A -> U+E101
  ["ef82a8", :undef], # [NEWSPAPER] U+E58B -> "[新聞]"
  ["eebe81", "ee858b"], # [SATELLITE ANTENNA] U+E4A8 -> U+E14B
  ["ef82af", :undef], # [OUTBOX TRAY] U+E592 -> "[送信BOX]"
  ["ef82b0", :undef], # [INBOX TRAY] U+E593 -> "[受信BOX]"
  ["eebfb8", "ee8492"], # [PACKAGE] U+E51F -> U+E112
  ["eeb5b5", "ee8483"], # [E-MAIL SYMBOL] U+EB71 -> U+E103
  ["ef8394", :undef], # [INPUT SYMBOL FOR LATIN CAPITAL LETTERS] U+EAFD -> "[ABCD]"
  ["ef8395", :undef], # [INPUT SYMBOL FOR LATIN SMALL LETTERS] U+EAFE -> "[abcd]"
  ["ef8396", :undef], # [INPUT SYMBOL FOR NUMBERS] U+EAFF -> "[1234]"
  ["ef8397", :undef], # [INPUT SYMBOL FOR SYMBOLS] U+EB00 -> "[記号]"
  ["eeb599", :undef], # [INPUT SYMBOL FOR LATIN LETTERS] U+EB55 -> "[ABC]"
  ["ef839a", :undef], # [BLACK NIB] U+EB03 -> "[ペン]"
  ["ef83a8", "ee808c"], # [PERSONAL COMPUTER] U+E5B8 -> U+E00C
  ["eebdb9", "ee8c81"], # [PENCIL] U+E4A1 -> U+E301
  ["eebdb8", :undef], # [PAPERCLIP] U+E4A0 -> "[クリップ]"
  ["eeb181", "ee849e"], # [BRIEFCASE] U+E5CE -> U+E11E
  ["ef829f", "ee8c96"], # [MINIDISC] U+E582 -> U+E316
  ["ef81be", "ee8c96"], # [FLOPPY DISK] U+E562 -> U+E316
  ["eebfa5", "ee84a6"], # [OPTICAL DISC] U+E50C -> U+E126
  ["eebfaf", "ee8c93"], # [BLACK SCISSORS] U+E516 -> U+E313
  ["ef81bc", :undef], # [ROUND PUSHPIN] U+E560 -> "[画びょう]"
  ["ef81bd", "ee8c81"], # [PAGE WITH CURL] U+E561 -> U+E301
  ["ef8286", "ee8c81"], # [PAGE FACING UP] U+E569 -> U+E301
  ["ef8280", :undef], # [CALENDAR] U+E563 -> "[カレンダー]"
  ["ef82ac", :undef], # [FILE FOLDER] U+E58F -> "[フォルダ]"
  ["ef82ad", :undef], # [OPEN FILE FOLDER] U+E590 -> "[フォルダ]"
  ["ef8288", "ee8588"], # [NOTEBOOK] U+E56B -> U+E148
  ["eebdb7", "ee8588"], # [OPEN BOOK] U+E49F -> U+E148
  ["eebdb5", "ee8588"], # [NOTEBOOK WITH DECORATIVE COVER] U+E49D -> U+E148
  ["ef8285", "ee8588"], # [CLOSED BOOK] U+E568 -> U+E148
  ["ef8282", "ee8588"], # [GREEN BOOK] U+E565 -> U+E148
  ["ef8283", "ee8588"], # [BLUE BOOK] U+E566 -> U+E148
  ["ef8284", "ee8588"], # [ORANGE BOOK] U+E567 -> U+E148
  ["ef828c", "ee8588"], # [BOOKS] U+E56F -> U+E148
  ["eebfb6", :undef], # [NAME BADGE] U+E51D -> "[名札]"
  ["ef81bb", :undef], # [SCROLL] U+E55F -> "[スクロール]"
  ["ef8281", "ee8c81"], # [CLIPBOARD] U+E564 -> U+E301
  ["ef8287", :undef], # [TEAR-OFF CALENDAR] U+E56A -> "[カレンダー]"
  ["ef8291", "ee858a"], # [BAR CHART] U+E574 -> U+E14A
  ["ef8292", "ee858a"], # [CHART WITH UPWARDS TREND] U+E575 -> U+E14A
  ["ef8293", :undef], # [CHART WITH DOWNWARDS TREND] U+E576 -> "[グラフ]"
  ["ef8289", "ee8588"], # [CARD INDEX] U+E56C -> U+E148
  ["ef828a", :undef], # [PUSHPIN] U+E56D -> "[画びょう]"
  ["ef828b", "ee8588"], # [LEDGER] U+E56E -> U+E148
  ["ef828d", :undef], # [STRAIGHT RULER] U+E570 -> "[定規]"
  ["eebdba", :undef], # [TRIANGULAR RULER] U+E4A2 -> "[三角定規]"
  ["ef83a2", "ee8c81"], # [BOOKMARK TABS] U+EB0B -> U+E301
  ["eebe93", "ee8096"], # [BASEBALL] U+E4BA -> U+E016
  ["ef82b6", "ee8094"], # [FLAG IN HOLE] U+E599 -> U+E014
  ["eebe90", "ee8095"], # [TENNIS RACQUET AND BALL] U+E4B7 -> U+E015
  ["eebe8f", "ee8098"], # [SOCCER BALL] U+E4B6 -> U+E018
  ["eeb280", "ee8093"], # [SKI AND SKI BOOT] U+EAAC -> U+E013
  ["ef82b7", "ee90aa"], # [BASKETBALL AND HOOP] U+E59A -> U+E42A
  ["eebe92", "ee84b2"], # [CHEQUERED FLAG] U+E4B9 -> U+E132
  ["eebe91", :undef], # [SNOWBOARDER] U+E4B8 -> "[スノボ]"
  ["eebd83", "ee8495"], # [RUNNER] U+E46B -> U+E115
  ["eeb585", "ee8097"], # [SURFER] U+EB41 -> U+E017
  ["eeb186", "ee84b1"], # [TROPHY] U+E5D3 -> U+E131
  ["eebe94", "ee90ab"], # [AMERICAN FOOTBALL] U+E4BB -> U+E42B
  ["eeb2b2", "ee90ad"], # [SWIMMER] U+EADE -> U+E42D
  ["eebe8e", "ee809e"], # [TRAIN] U+E4B5 -> U+E01E
  ["ef83ac", "ee90b4"], # [METRO] U+E5BC -> U+E434
  ["eebe89", "ee809f"], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+E4B0 -> U+E01F
  ["eebe8a", "ee809b"], # [AUTOMOBILE] U+E4B1 -> U+E01B
  ["eebe88", "ee8599"], # [ONCOMING BUS] U+E4AF -> U+E159
  ["eebe80", "ee8590"], # [BUS STOP] U+E4A7 -> U+E150
  ["eeb195", "ee8882"], # [SHIP] U+EA82 -> U+E202
  ["eebe8c", "ee809d"], # [AIRPLANE] U+E4B3 -> U+E01D
  ["eebe8d", "ee809c"], # [SAILBOAT] U+E4B4 -> U+E01C
  ["eeb5b1", "ee80b9"], # [STATION] U+EB6D -> U+E039
  ["ef83b8", "ee848d"], # [ROCKET] U+E5C8 -> U+E10D
  ["eebe8b", "ee90af"], # [DELIVERY TRUCK] U+E4B2 -> U+E42F
  ["eeb2b3", "ee90b0"], # [FIRE ENGINE] U+EADF -> U+E430
  ["eeb2b4", "ee90b1"], # [AMBULANCE] U+EAE0 -> U+E431
  ["eeb2b5", "ee90b2"], # [POLICE CAR] U+EAE1 -> U+E432
  ["ef828e", "ee80ba"], # [FUEL PUMP] U+E571 -> U+E03A
  ["eebdbe", "ee858f"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E4A6 -> U+E14F
  ["eebd82", "ee858e"], # [HORIZONTAL TRAFFIC LIGHT] U+E46A -> U+E14E
  ["eeb18a", "ee84b7"], # [CONSTRUCTION SIGN] U+E5D7 -> U+E137
  ["eeb5b7", "ee90b2"], # [POLICE CARS REVOLVING LIGHT] U+EB73 -> U+E432
  ["eebe95", "ee84a3"], # [HOT SPRINGS] U+E4BC -> U+E123
  ["eeb183", "ee84a2"], # [TENT] U+E5D0 -> U+E122
  ["eebd85", "ee84a4"], # [FERRIS WHEEL] U+E46D -> U+E124
  ["eeb2b6", "ee90b3"], # [ROLLER COASTER] U+EAE2 -> U+E433
  ["eeb586", "ee8099"], # [FISHING POLE AND FISH] U+EB42 -> U+E019
  ["eebf9c", "ee80bc"], # [MICROPHONE] U+E503 -> U+E03C
  ["eebfb0", "ee80bd"], # [MOVIE CAMERA] U+E517 -> U+E03D
  ["eebfa1", "ee8c8a"], # [HEADPHONE] U+E508 -> U+E30A
  ["ef82b9", "ee9482"], # [ARTIST PALETTE] U+E59C -> U+E502
  ["eeb389", "ee9483"], # [TOP HAT] U+EAF5 -> U+E503
  ["ef82bb", :undef], # [CIRCUS TENT] U+E59E -> "[イベント]"
  ["eebdb6", "ee84a5"], # [TICKET] U+E49E -> U+E125
  ["eebe97", "ee8ca4"], # [CLAPPER BOARD] U+E4BE -> U+E324
  ["ef82ba", "ee9483"], # [PERFORMING ARTS] U+E59D -> U+E503
  ["eebe9f", :undef], # [VIDEO GAME] U+E4C6 -> "[ゲーム]"
  ["eeb184", "ee84ad"], # [MAHJONG TILE RED DRAGON] U+E5D1 -> U+E12D
  ["eebe9e", "ee84b0"], # [DIRECT HIT] U+E4C5 -> U+E130
  ["eebd86", "ee84b3"], # [SLOT MACHINE] U+E46E -> U+E133
  ["eeb2b1", "ee90ac"], # [BILLIARDS] U+EADD -> U+E42C
  ["eebea1", :undef], # [GAME DIE] U+E4C8 -> "[サイコロ]"
  ["eeb587", :undef], # [BOWLING] U+EB43 -> "[ボーリング]"
  ["eeb5b2", :undef], # [FLOWER PLAYING CARDS] U+EB6E -> "[花札]"
  ["eeb5b3", :undef], # [PLAYING CARD BLACK JOKER] U+EB6F -> "[ジョーカー]"
  ["ef83ae", "ee80be"], # [MUSICAL NOTE] U+E5BE -> U+E03E
  ["eebf9e", "ee8ca6"], # [MULTIPLE MUSICAL NOTES] U+E505 -> U+E326
  ["eebf9f", "ee8181"], # [GUITAR] U+E506 -> U+E041
  ["eeb584", :undef], # [MUSICAL KEYBOARD] U+EB40 -> "[ピアノ]"
  ["eeb2b0", "ee8182"], # [TRUMPET] U+EADC -> U+E042
  ["eebfa0", :undef], # [VIOLIN] U+E507 -> "[バイオリン]"
  ["eeb2a0", "ee8ca6"], # [MUSICAL SCORE] U+EACC -> U+E326
  ["eebfae", "ee8088"], # [CAMERA] U+E515 -> U+E008
  ["ef829b", "ee80bd"], # [VIDEO CAMERA] U+E57E -> U+E03D
  ["eebf9b", "ee84aa"], # [TELEVISION] U+E502 -> U+E12A
  ["ef83a9", "ee84a8"], # [RADIO] U+E5B9 -> U+E128
  ["ef829d", "ee84a9"], # [VIDEOCASSETTE] U+E580 -> U+E129
  ["eebf84", "ee8083"], # [KISS MARK] U+E4EB -> U+E003
  ["eeb5bc", "ee8483ee8ca8"], # [LOVE LETTER] U+EB78 -> U+E103 U+E328
  ["eebfad", "ee80b4"], # [RING] U+E514 -> U+E034
  ["ef83ba", "ee8491"], # [KISS] U+E5CA -> U+E111
  ["eeb1a8", "ee8c86"], # [BOUQUET] U+EA95 -> U+E306
  ["eeb2ae", "ee90a5"], # [COUPLE WITH HEART] U+EADA -> U+E425
  ["eeb196", "ee8887"], # [NO ONE UNDER EIGHTEEN SYMBOL] U+EA83 -> U+E207
  ["ef81b4", "ee898e"], # [COPYRIGHT SIGN] U+E558 -> U+E24E
  ["ef81b5", "ee898f"], # [REGISTERED SIGN] U+E559 -> U+E24F
  ["ef81aa", "ee94b7"], # [TRADE MARK SIGN] U+E54E -> U+E537
  ["ef818f", :undef], # [INFORMATION SOURCE] U+E533 -> "[ｉ]"
  ["eeb689", "ee8890"], # [HASH KEY] U+EB84 -> U+E210
  ["eebfbb", "ee889c"], # [KEYCAP 1] U+E522 -> U+E21C
  ["eebfbc", "ee889d"], # [KEYCAP 2] U+E523 -> U+E21D
  ["ef8180", "ee889e"], # [KEYCAP 3] U+E524 -> U+E21E
  ["ef8181", "ee889f"], # [KEYCAP 4] U+E525 -> U+E21F
  ["ef8182", "ee88a0"], # [KEYCAP 5] U+E526 -> U+E220
  ["ef8183", "ee88a1"], # [KEYCAP 6] U+E527 -> U+E221
  ["ef8184", "ee88a2"], # [KEYCAP 7] U+E528 -> U+E222
  ["ef8185", "ee88a3"], # [KEYCAP 8] U+E529 -> U+E223
  ["ef8186", "ee88a4"], # [KEYCAP 9] U+E52A -> U+E224
  ["ef8389", "ee88a5"], # [KEYCAP 0] U+E5AC -> U+E225
  ["ef8187", :undef], # [KEYCAP TEN] U+E52B -> "[10]"
  ["eeb197", "ee888b"], # [ANTENNA WITH BARS] U+EA84 -> U+E20B
  ["eeb1a3", "ee8990"], # [VIBRATION MODE] U+EA90 -> U+E250
  ["eeb1a4", "ee8991"], # [MOBILE PHONE OFF] U+EA91 -> U+E251
  ["eebeaf", "ee84a0"], # [HAMBURGER] U+E4D6 -> U+E120
  ["eebeae", "ee8d82"], # [RICE BALL] U+E4D5 -> U+E342
  ["eebea9", "ee8186"], # [SHORTCAKE] U+E4D0 -> U+E046
  ["ef8391", "ee8d80"], # [STEAMING BOWL] U+E5B4 -> U+E340
  ["eeb283", "ee8cb9"], # [BREAD] U+EAAF -> U+E339
  ["eebeaa", "ee8587"], # [COOKING] U+E4D1 -> U+E147
  ["eeb284", "ee8cba"], # [SOFT ICE CREAM] U+EAB0 -> U+E33A
  ["eeb285", "ee8cbb"], # [FRENCH FRIES] U+EAB1 -> U+E33B
  ["eeb286", "ee8cbc"], # [DANGO] U+EAB2 -> U+E33C
  ["eeb287", "ee8cbd"], # [RICE CRACKER] U+EAB3 -> U+E33D
  ["eeb288", "ee8cbe"], # [COOKED RICE] U+EAB4 -> U+E33E
  ["eeb289", "ee8cbf"], # [SPAGHETTI] U+EAB5 -> U+E33F
  ["eeb28a", "ee8d81"], # [CURRY AND RICE] U+EAB6 -> U+E341
  ["eeb28b", "ee8d83"], # [ODEN] U+EAB7 -> U+E343
  ["eeb28c", "ee8d84"], # [SUSHI] U+EAB8 -> U+E344
  ["eeb291", "ee8d8c"], # [BENTO BOX] U+EABD -> U+E34C
  ["eeb292", "ee8d8d"], # [POT OF FOOD] U+EABE -> U+E34D
  ["eeb2be", "ee90bf"], # [SHAVED ICE] U+EAEA -> U+E43F
  ["eebe9d", :undef], # [MEAT ON BONE] U+E4C4 -> "[肉]"
  ["eebf86", :undef], # [FISH CAKE WITH SWIRL DESIGN] U+E4ED -> "[なると]"
  ["eeb3bb", :undef], # [ROASTED SWEET POTATO] U+EB3A -> "[やきいも]"
  ["eeb3bc", :undef], # [SLICE OF PIZZA] U+EB3B -> "[ピザ]"
  ["eeb580", :undef], # [POULTRY LEG] U+EB3C -> "[チキン]"
  ["eeb58e", :undef], # [ICE CREAM] U+EB4A -> "[アイスクリーム]"
  ["eeb58f", :undef], # [DOUGHNUT] U+EB4B -> "[ドーナツ]"
  ["eeb590", :undef], # [COOKIE] U+EB4C -> "[クッキー]"
  ["eeb591", :undef], # [CHOCOLATE BAR] U+EB4D -> "[チョコ]"
  ["eeb592", :undef], # [CANDY] U+EB4E -> "[キャンディ]"
  ["eeb593", :undef], # [LOLLIPOP] U+EB4F -> "[キャンディ]"
  ["eeb59a", :undef], # [CUSTARD] U+EB56 -> "[プリン]"
  ["eeb59d", :undef], # [HONEY POT] U+EB59 -> "[ハチミツ]"
  ["eeb5b4", :undef], # [FRIED SHRIMP] U+EB70 -> "[エビフライ]"
  ["eebe85", "ee8183"], # [FORK AND KNIFE] U+E4AC -> U+E043
  ["ef82b4", "ee8185"], # [HOT BEVERAGE] U+E597 -> U+E045
  ["eebe9b", "ee8184"], # [COCKTAIL GLASS] U+E4C2 -> U+E044
  ["eebe9c", "ee8187"], # [BEER MUG] U+E4C3 -> U+E047
  ["eeb282", "ee8cb8"], # [TEACUP WITHOUT HANDLE] U+EAAE -> U+E338
  ["eeb1aa", "ee8c8b"], # [SAKE BOTTLE AND CUP] U+EA97 -> U+E30B
  ["eebe9a", "ee8184"], # [WINE GLASS] U+E4C1 -> U+E044
  ["eeb1ab", "ee8c8c"], # [CLINKING BEER MUGS] U+EA98 -> U+E30C
  ["eeb582", "ee8184"], # [TROPICAL DRINK] U+EB3E -> U+E044
  ["ef81b1", "ee88b6"], # [NORTH EAST ARROW] U+E555 -> U+E236
  ["ef81a9", "ee88b8"], # [SOUTH EAST ARROW] U+E54D -> U+E238
  ["ef81a8", "ee88b7"], # [NORTH WEST ARROW] U+E54C -> U+E237
  ["ef81b2", "ee88b9"], # [SOUTH WEST ARROW] U+E556 -> U+E239
  ["eeb3ae", "ee88b6"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+EB2D -> U+E236
  ["eeb3af", "ee88b8"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+EB2E -> U+E238
  ["eeb5be", :undef], # [LEFT RIGHT ARROW] U+EB7A -> "⇔"
  ["eeb680", :undef], # [UP DOWN ARROW] U+EB7B -> "↑↓"
  ["ef819b", "ee88b2"], # [UPWARDS BLACK ARROW] U+E53F -> U+E232
  ["ef819c", "ee88b3"], # [DOWNWARDS BLACK ARROW] U+E540 -> U+E233
  ["ef81ae", "ee88b4"], # [BLACK RIGHTWARDS ARROW] U+E552 -> U+E234
  ["ef81af", "ee88b5"], # [LEFTWARDS BLACK ARROW] U+E553 -> U+E235
  ["ef818a", "ee88ba"], # [BLACK RIGHT-POINTING TRIANGLE] U+E52E -> U+E23A
  ["ef8189", "ee88bb"], # [BLACK LEFT-POINTING TRIANGLE] U+E52D -> U+E23B
  ["ef818c", "ee88bc"], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+E530 -> U+E23C
  ["ef818b", "ee88bd"], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+E52F -> U+E23D
  ["ef81a1", :undef], # [BLACK UP-POINTING DOUBLE TRIANGLE] U+E545 -> "▲"
  ["ef81a0", :undef], # [BLACK DOWN-POINTING DOUBLE TRIANGLE] U+E544 -> "▼"
  ["ef81b6", :undef], # [UP-POINTING RED TRIANGLE] U+E55A -> "▲"
  ["ef81b7", :undef], # [DOWN-POINTING RED TRIANGLE] U+E55B -> "▼"
  ["ef819f", :undef], # [UP-POINTING SMALL RED TRIANGLE] U+E543 -> "▲"
  ["ef819e", :undef], # [DOWN-POINTING SMALL RED TRIANGLE] U+E542 -> "▼"
  ["eeb281", "ee8cb2"], # [HEAVY LARGE CIRCLE] U+EAAD -> U+E332
  ["ef81ac", "ee8cb3"], # [CROSS MARK] U+E550 -> U+E333
  ["ef81ad", "ee8cb3"], # [NEGATIVE SQUARED CROSS MARK] U+E551 -> U+E333
  ["eebd9a", "ee80a1"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E482 -> U+E021
  ["eeb3b0", :undef], # [EXCLAMATION QUESTION MARK] U+EB2F -> "！？"
  ["eeb3b1", :undef], # [DOUBLE EXCLAMATION MARK] U+EB30 -> "！！"
  ["eebd9b", "ee80a0"], # [BLACK QUESTION MARK ORNAMENT] U+E483 -> U+E020
  ["eeb3b2", :undef], # [CURLY LOOP] U+EB31 -> "～"
  ["ef82b2", "ee80a2"], # [HEAVY BLACK HEART] U+E595 -> U+E022
  ["eeb5b9", "ee8ca7"], # [BEATING HEART] U+EB75 -> U+E327
  ["eebd8f", "ee80a3"], # [BROKEN HEART] U+E477 -> U+E023
  ["eebd90", "ee8ca7"], # [TWO HEARTS] U+E478 -> U+E327
  ["eeb1b9", "ee8ca7"], # [SPARKLING HEART] U+EAA6 -> U+E327
  ["eebf83", "ee8ca9"], # [HEART WITH ARROW] U+E4EA -> U+E329
  ["eeb1ba", "ee8caa"], # [BLUE HEART] U+EAA7 -> U+E32A
  ["eeb1bb", "ee8cab"], # [GREEN HEART] U+EAA8 -> U+E32B
  ["eeb1bc", "ee8cac"], # [YELLOW HEART] U+EAA9 -> U+E32C
  ["eeb1bd", "ee8cad"], # [PURPLE HEART] U+EAAA -> U+E32D
  ["eeb598", "ee90b7"], # [HEART WITH RIBBON] U+EB54 -> U+E437
  ["ef838c", "ee8ca7"], # [REVOLVING HEARTS] U+E5AF -> U+E327
  ["eeb1b8", "ee888c"], # [BLACK HEART SUIT] U+EAA5 -> U+E20C
  ["ef82be", "ee888e"], # [BLACK SPADE SUIT] U+E5A1 -> U+E20E
  ["ef82bf", "ee888d"], # [BLACK DIAMOND SUIT] U+E5A2 -> U+E20D
  ["ef8380", "ee888f"], # [BLACK CLUB SUIT] U+E5A3 -> U+E20F
  ["eebd95", "ee8c8e"], # [SMOKING SYMBOL] U+E47D -> U+E30E
  ["eebd96", "ee8888"], # [NO SMOKING SYMBOL] U+E47E -> U+E208
  ["eebd97", "ee888a"], # [WHEELCHAIR SYMBOL] U+E47F -> U+E20A
  ["eeb3ad", :undef], # [TRIANGULAR FLAG ON POST] U+EB2C -> "[旗]"
  ["eebd99", "ee8992"], # [WARNING SIGN] U+E481 -> U+E252
  ["eebd9c", "ee84b7"], # [NO ENTRY] U+E484 -> U+E137
  ["eeb5bd", :undef], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+EB79 -> "↑↓"
  ["eebe87", "ee84b6"], # [BICYCLE] U+E4AE -> U+E136
  ["eeb5b6", "ee8881"], # [PEDESTRIAN] U+EB72 -> U+E201
  ["eeb18b", "ee84bf"], # [BATH] U+E5D8 -> U+E13F
  ["eebdbd", "ee8591"], # [RESTROOM] U+E4A5 -> U+E151
  ["ef819d", :undef], # [NO ENTRY SIGN] U+E541 -> "[禁止]"
  ["ef81b3", :undef], # [HEAVY CHECK MARK] U+E557 -> "[チェックマーク]"
  ["ef8388", :undef], # [SQUARED CL] U+E5AB -> "[CL]"
  ["eeb198", "ee8894"], # [SQUARED COOL] U+EA85 -> U+E214
  ["ef8295", :undef], # [SQUARED FREE] U+E578 -> "[FREE]"
  ["eeb19b", "ee88a9"], # [SQUARED ID] U+EA88 -> U+E229
  ["ef83a5", "ee8892"], # [SQUARED NEW] U+E5B5 -> U+E212
  ["ef838a", "ee898d"], # [SQUARED OK] U+E5AD -> U+E24D
  ["eebf81", :undef], # [SQUARED SOS] U+E4E8 -> "[SOS]"
  ["eebfa8", "ee8893"], # [SQUARED UP WITH EXCLAMATION MARK] U+E50F -> U+E213
  ["eeb185", "ee84ae"], # [SQUARED VS] U+E5D2 -> U+E12E
  ["eeb19a", "ee88a8"], # [SQUARED KATAKANA SA] U+EA87 -> U+E228
  ["eeb19d", "ee88ab"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+EA8A -> U+E22B
  ["eeb19c", "ee88aa"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+EA89 -> U+E22A
  ["eeb199", "ee88a7"], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+EA86 -> U+E227
  ["eeb19e", "ee88ac"], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+EA8B -> U+E22C
  ["eeb19f", "ee88ad"], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+EA8C -> U+E22D
  ["eebf8a", "ee8c95"], # [CIRCLED IDEOGRAPH SECRET] U+E4F1 -> U+E315
  ["eeb1ac", "ee8c8d"], # [CIRCLED IDEOGRAPH CONGRATULATION] U+EA99 -> U+E30D
  ["eebf90", "ee88a6"], # [CIRCLED IDEOGRAPH ADVANTAGE] U+E4F7 -> U+E226
  ["ef8398", :undef], # [CIRCLED IDEOGRAPH ACCEPT] U+EB01 -> "[可]"
  ["ef8198", :undef], # [HEAVY PLUS SIGN] U+E53C -> "[＋]"
  ["ef8199", :undef], # [HEAVY MINUS SIGN] U+E53D -> "[－]"
  ["ef81ab", "ee8cb3"], # [HEAVY MULTIPLICATION X] U+E54F -> U+E333
  ["ef81b0", :undef], # [HEAVY DIVISION SIGN] U+E554 -> "[÷]"
  ["eebd8e", "ee848f"], # [ELECTRIC LIGHT BULB] U+E476 -> U+E10F
  ["eebebe", "ee8cb4"], # [ANGER SYMBOL] U+E4E5 -> U+E334
  ["eebd92", "ee8c91"], # [BOMB] U+E47A -> U+E311
  ["eebd8d", "ee84bc"], # [SLEEPING SYMBOL] U+E475 -> U+E13C
  ["ef838d", :undef], # [COLLISION SYMBOL] U+E5B0 -> "[ドンッ]"
  ["ef838e", "ee8cb1"], # [SPLASHING SWEAT SYMBOL] U+E5B1 -> U+E331
  ["eebebf", "ee8cb1"], # [DROP OF WATER] U+E4E6 -> U+E331
  ["eebf8d", "ee8cb0"], # [DASH SYMBOL] U+E4F4 -> U+E330
  ["eebf8e", "ee819a"], # [PILE OF POO] U+E4F5 -> U+E05A
  ["eebf82", "ee858c"], # [FLEXED BICEPS] U+E4E9 -> U+E14C
  ["eeb5a0", "ee9087"], # [DIZZY SYMBOL] U+EB5C -> U+E407
  ["eebf96", :undef], # [SPEECH BALLOON] U+E4FD -> "[フキダシ]"
  ["eeb1be", "ee8cae"], # [SPARKLES] U+EAAB -> U+E32E
  ["eebd91", "ee8885"], # [EIGHT POINTED BLACK STAR] U+E479 -> U+E205
  ["ef819a", "ee8886"], # [EIGHT SPOKED ASTERISK] U+E53E -> U+E206
  ["ef8196", "ee8899"], # [MEDIUM WHITE CIRCLE] U+E53A -> U+E219
  ["ef8197", "ee8899"], # [MEDIUM BLACK CIRCLE] U+E53B -> U+E219
  ["ef81a6", "ee8899"], # [LARGE RED CIRCLE] U+E54A -> U+E219
  ["ef81a7", "ee889a"], # [LARGE BLUE CIRCLE] U+E54B -> U+E21A
  ["eebda3", "ee8caf"], # [WHITE MEDIUM STAR] U+E48B -> U+E32F
  ["ef81a4", "ee889b"], # [WHITE LARGE SQUARE] U+E548 -> U+E21B
  ["ef81a5", "ee889a"], # [BLACK LARGE SQUARE] U+E549 -> U+E21A
  ["ef818d", "ee889b"], # [WHITE SMALL SQUARE] U+E531 -> U+E21B
  ["ef818e", "ee889a"], # [BLACK SMALL SQUARE] U+E532 -> U+E21A
  ["ef8190", "ee889b"], # [WHITE MEDIUM SMALL SQUARE] U+E534 -> U+E21B
  ["ef8191", "ee889a"], # [BLACK MEDIUM SMALL SQUARE] U+E535 -> U+E21A
  ["ef8194", "ee889b"], # [WHITE MEDIUM SQUARE] U+E538 -> U+E21B
  ["ef8195", "ee889a"], # [BLACK MEDIUM SQUARE] U+E539 -> U+E21A
  ["ef81a2", "ee889b"], # [LARGE ORANGE DIAMOND] U+E546 -> U+E21B
  ["ef81a3", "ee889b"], # [LARGE BLUE DIAMOND] U+E547 -> U+E21B
  ["ef8192", "ee889b"], # [SMALL ORANGE DIAMOND] U+E536 -> U+E21B
  ["ef8193", "ee889b"], # [SMALL BLUE DIAMOND] U+E537 -> U+E21B
  ["eebd84", "ee8cae"], # [SPARKLE] U+E46C -> U+E32E
  ["eebf89", :undef], # [WHITE FLOWER] U+E4F0 -> "[花丸]"
  ["eebf8b", :undef], # [HUNDRED POINTS SYMBOL] U+E4F2 -> "[100点]"
  ["ef81b9", :undef], # [LEFTWARDS ARROW WITH HOOK] U+E55D -> "←┘"
  ["ef81b8", :undef], # [RIGHTWARDS ARROW WITH HOOK] U+E55C -> "└→"
  ["ef83a4", :undef], # [CLOCKWISE DOWNWARDS AND UPWARDS OPEN CIRCLE ARROWS] U+EB0D -> "↑↓"
  ["eebfaa", "ee8581"], # [SPEAKER WITH THREE SOUND WAVES] U+E511 -> U+E141
  ["ef82a1", :undef], # [BATTERY] U+E584 -> "[電池]"
  ["ef82a6", :undef], # [ELECTRIC PLUG] U+E589 -> "[コンセント]"
  ["eebfb1", "ee8494"], # [LEFT-POINTING MAGNIFYING GLASS] U+E518 -> U+E114
  ["ef839c", "ee8494"], # [RIGHT-POINTING MAGNIFYING GLASS] U+EB05 -> U+E114
  ["eebfb5", "ee8584"], # [LOCK] U+E51C -> U+E144
  ["ef83a3", "ee8584"], # [LOCK WITH INK PEN] U+EB0C -> U+E144
  ["ef8393", "ee8584"], # [CLOSED LOCK WITH KEY] U+EAFC -> U+E144
  ["eebfb2", "ee80bf"], # [KEY] U+E519 -> U+E03F
  ["eebfab", "ee8ca5"], # [BELL] U+E512 -> U+E325
  ["ef8399", :undef], # [BALLOT BOX WITH CHECK] U+EB02 -> "[チェックマーク]"
  ["ef839b", :undef], # [RADIO BUTTON] U+EB04 -> "[ラジオボタン]"
  ["ef839e", :undef], # [BOOKMARK] U+EB07 -> "[ブックマーク]"
  ["ef82a7", :undef], # [LINK SYMBOL] U+E58A -> "[リンク]"
  ["ef839d", "ee88b5"], # [BACK WITH LEFTWARDS ARROW ABOVE] U+EB06 -> U+E235
  ["ef82a9", :undef], # [EM SPACE] U+E58C -> U+3013 (GETA)
  ["ef82aa", :undef], # [EN SPACE] U+E58D -> U+3013 (GETA)
  ["ef82ab", :undef], # [FOUR-PER-EM SPACE] U+E58E -> U+3013 (GETA)
  ["ef81ba", :undef], # [WHITE HEAVY CHECK MARK] U+E55E -> "[チェックマーク]"
  ["eeb688", "ee8090"], # [RAISED FIST] U+EB83 -> U+E010
  ["ef8384", "ee8092"], # [RAISED HAND] U+E5A7 -> U+E012
  ["ef8383", "ee8091"], # [VICTORY HAND] U+E5A6 -> U+E011
  ["eebf8c", "ee808d"], # [FISTED HAND SIGN] U+E4F3 -> U+E00D
  ["eebf92", "ee808e"], # [THUMBS UP SIGN] U+E4F9 -> U+E00E
  ["eebf8f", "ee808f"], # [WHITE UP POINTING INDEX] U+E4F6 -> U+E00F
  ["eeb1a0", "ee88ae"], # [WHITE UP POINTING BACKHAND INDEX] U+EA8D -> U+E22E
  ["eeb1a1", "ee88af"], # [WHITE DOWN POINTING BACKHAND INDEX] U+EA8E -> U+E22F
  ["eebf98", "ee88b0"], # [WHITE LEFT POINTING BACKHAND INDEX] U+E4FF -> U+E230
  ["eebf99", "ee88b1"], # [WHITE RIGHT POINTING BACKHAND INDEX] U+E500 -> U+E231
  ["eeb2aa", "ee909e"], # [WAVING HAND SIGN] U+EAD6 -> U+E41E
  ["eeb2a7", "ee909f"], # [CLAPPING HANDS SIGN] U+EAD3 -> U+E41F
  ["eeb2a8", "ee90a0"], # [OK HAND SIGN] U+EAD4 -> U+E420
  ["eeb2a9", "ee90a1"], # [THUMBS DOWN SIGN] U+EAD5 -> U+E421
  ["ef8294", :undef], # [EMOJI COMPATIBILITY SYMBOL-37] U+E577 -> "[EZ]"
  ["ef838f", :undef], # [EMOJI COMPATIBILITY SYMBOL-38] U+E5B2 -> "[ezplus]"
  ["eeb1b0", :undef], # [EMOJI COMPATIBILITY SYMBOL-39] U+EA9D -> "[EZナビ]"
  ["eeb5b8", :undef], # [EMOJI COMPATIBILITY SYMBOL-40] U+EB74 -> "[EZムービー]"
  ["eeb686", :undef], # [EMOJI COMPATIBILITY SYMBOL-41] U+EB81 -> "[Cメール]"
  ["eeb68e", :undef], # [EMOJI COMPATIBILITY SYMBOL-42] U+EB89 -> "[Java]"
  ["eeb68f", :undef], # [EMOJI COMPATIBILITY SYMBOL-43] U+EB8A -> "[BREW]"
  ["eeb690", :undef], # [EMOJI COMPATIBILITY SYMBOL-44] U+EB8B -> "[EZ着うた]"
  ["eeb691", :undef], # [EMOJI COMPATIBILITY SYMBOL-45] U+EB8C -> "[EZナビ]"
  ["eeb692", :undef], # [EMOJI COMPATIBILITY SYMBOL-46] U+EB8D -> "[WIN]"
  ["eeb693", :undef], # [EMOJI COMPATIBILITY SYMBOL-47] U+EB8E -> "[プレミアム]"
  ["ef8392", :undef], # [EMOJI COMPATIBILITY SYMBOL-48] U+EAFB -> "[オープンウェブ]"
  ["ef82a3", :undef], # [EMOJI COMPATIBILITY SYMBOL-49] U+E586 -> "[PDC]"
  ["ef8188", :undef], # [EMOJI COMPATIBILITY SYMBOL-66] U+E52C -> "[Q]"
]

EMOJI_EXCHANGE_TBL['UTF8-KDDI']['UTF-8'] = [
  # for documented codepoints
  ["ee9288", "e29880"], # [BLACK SUN WITH RAYS] U+E488 -> U+2600
  ["ee928d", "e29881"], # [CLOUD] U+E48D -> U+2601
  ["ee928c", "e29894"], # [UMBRELLA WITH RAIN DROPS] U+E48C -> U+2614
  ["ee9285", "e29b84"], # [SNOWMAN WITHOUT SNOW] U+E485 -> U+26C4
  ["ee9287", "e29aa1"], # [HIGH VOLTAGE SIGN] U+E487 -> U+26A1
  ["ee91a9", :undef], # [CYCLONE] U+E469 -> U+1F300
  ["ee9698", :undef], # [FOGGY] U+E598 -> U+1F301
  ["eeaba8", :undef], # [CLOSED UMBRELLA] U+EAE8 -> U+1F302
  ["eeabb1", :undef], # [NIGHT WITH STARS] U+EAF1 -> U+1F303
  ["eeabb4", :undef], # [SUNRISE] U+EAF4 -> U+1F305
  ["ee979a", :undef], # [CITYSCAPE AT DUSK] U+E5DA -> "[夕焼け]"
  ["eeabb2", :undef], # [RAINBOW] U+EAF2 -> U+1F308
  ["ee928a", "e29d84"], # [SNOWFLAKE] U+E48A -> U+2744
  ["ee928e", "e29b85"], # [SUN BEHIND CLOUD] U+E48E -> U+26C5
  ["ee92bf", :undef], # [BRIDGE AT NIGHT] U+E4BF -> U+1F309
  ["eeadbc", :undef], # [WATER WAVE] U+EB7C -> U+1F30A
  ["eead93", :undef], # [VOLCANO] U+EB53 -> U+1F30B
  ["eead9f", :undef], # [MILKY WAY] U+EB5F -> U+1F30C
  ["ee96b3", :undef], # [EARTH GLOBE ASIA-AUSTRALIA] U+E5B3 -> U+1F30F
  ["ee96a8", :undef], # [NEW MOON SYMBOL] U+E5A8 -> U+1F311
  ["ee96a9", :undef], # [WAXING GIBBOUS MOON SYMBOL] U+E5A9 -> U+1F314
  ["ee96aa", :undef], # [FIRST QUARTER MOON SYMBOL] U+E5AA -> U+1F313
  ["ee9286", :undef], # [CRESCENT MOON] U+E486 -> U+1F319
  ["ee9289", :undef], # [FIRST QUARTER MOON WITH FACE] U+E489 -> U+1F31B
  ["ee91a8", :undef], # [SHOOTING STAR] U+E468 -> U+1F320
  ["ee95ba", "e28c9a"], # [WATCH] U+E57A -> U+231A
  ["ee95bb", "e28c9b"], # [HOURGLASS] U+E57B -> U+231B
  ["ee9694", :undef], # [ALARM CLOCK] U+E594 -> U+23F0
  ["ee91bc", :undef], # [HOURGLASS WITH FLOWING SAND] U+E47C -> U+23F3
  ["ee928f", "e29988"], # [ARIES] U+E48F -> U+2648
  ["ee9290", "e29989"], # [TAURUS] U+E490 -> U+2649
  ["ee9291", "e2998a"], # [GEMINI] U+E491 -> U+264A
  ["ee9292", "e2998b"], # [CANCER] U+E492 -> U+264B
  ["ee9293", "e2998c"], # [LEO] U+E493 -> U+264C
  ["ee9294", "e2998d"], # [VIRGO] U+E494 -> U+264D
  ["ee9295", "e2998e"], # [LIBRA] U+E495 -> U+264E
  ["ee9296", "e2998f"], # [SCORPIUS] U+E496 -> U+264F
  ["ee9297", "e29990"], # [SAGITTARIUS] U+E497 -> U+2650
  ["ee9298", "e29991"], # [CAPRICORN] U+E498 -> U+2651
  ["ee9299", "e29992"], # [AQUARIUS] U+E499 -> U+2652
  ["ee929a", "e29993"], # [PISCES] U+E49A -> U+2653
  ["ee929b", :undef], # [OPHIUCHUS] U+E49B -> U+26CE
  ["ee9493", :undef], # [FOUR LEAF CLOVER] U+E513 -> U+1F340
  ["ee93a4", :undef], # [TULIP] U+E4E4 -> U+1F337
  ["eeadbd", :undef], # [SEEDLING] U+EB7D -> U+1F331
  ["ee938e", :undef], # [MAPLE LEAF] U+E4CE -> U+1F341
  ["ee938a", :undef], # [CHERRY BLOSSOM] U+E4CA -> U+1F338
  ["ee96ba", :undef], # [ROSE] U+E5BA -> U+1F339
  ["ee978d", :undef], # [FALLEN LEAF] U+E5CD -> U+1F342
  ["eeaa94", :undef], # [HIBISCUS] U+EA94 -> U+1F33A
  ["ee93a3", :undef], # [SUNFLOWER] U+E4E3 -> U+1F33B
  ["ee93a2", :undef], # [PALM TREE] U+E4E2 -> U+1F334
  ["eeaa96", :undef], # [CACTUS] U+EA96 -> U+1F335
  ["eeacb6", :undef], # [EAR OF MAIZE] U+EB36 -> U+1F33D
  ["eeacb7", :undef], # [MUSHROOM] U+EB37 -> U+1F344
  ["eeacb8", :undef], # [CHESTNUT] U+EB38 -> U+1F330
  ["eead89", :undef], # [BLOSSOM] U+EB49 -> U+1F33C
  ["eeae82", :undef], # [HERB] U+EB82 -> U+1F33F
  ["ee9392", :undef], # [CHERRIES] U+E4D2 -> U+1F352
  ["eeacb5", :undef], # [BANANA] U+EB35 -> U+1F34C
  ["eeaab9", :undef], # [RED APPLE] U+EAB9 -> U+1F34E
  ["eeaaba", :undef], # [TANGERINE] U+EABA -> U+1F34A
  ["ee9394", :undef], # [STRAWBERRY] U+E4D4 -> U+1F353
  ["ee938d", :undef], # [WATERMELON] U+E4CD -> U+1F349
  ["eeaabb", :undef], # [TOMATO] U+EABB -> U+1F345
  ["eeaabc", :undef], # [AUBERGINE] U+EABC -> U+1F346
  ["eeacb2", :undef], # [MELON] U+EB32 -> U+1F348
  ["eeacb3", :undef], # [PINEAPPLE] U+EB33 -> U+1F34D
  ["eeacb4", :undef], # [GRAPES] U+EB34 -> U+1F347
  ["eeacb9", :undef], # [PEACH] U+EB39 -> U+1F351
  ["eead9a", :undef], # [GREEN APPLE] U+EB5A -> U+1F34F
  ["ee96a4", :undef], # [EYES] U+E5A4 -> U+1F440
  ["ee96a5", :undef], # [EAR] U+E5A5 -> U+1F442
  ["eeab90", :undef], # [NOSE] U+EAD0 -> U+1F443
  ["eeab91", :undef], # [MOUTH] U+EAD1 -> U+1F444
  ["eead87", :undef], # [TONGUE] U+EB47 -> U+1F445
  ["ee9489", :undef], # [LIPSTICK] U+E509 -> U+1F484
  ["eeaaa0", :undef], # [NAIL POLISH] U+EAA0 -> U+1F485
  ["ee948b", :undef], # [FACE MASSAGE] U+E50B -> U+1F486
  ["eeaaa1", :undef], # [HAIRCUT] U+EAA1 -> U+1F487
  ["eeaaa2", :undef], # [BARBER POLE] U+EAA2 -> U+1F488
  ["ee93bc", :undef], # [BOY] U+E4FC -> U+1F466
  ["ee93ba", :undef], # [GIRL] U+E4FA -> U+1F467
  ["ee9481", :undef], # [FAMILY] U+E501 -> U+1F46A
  ["ee979d", :undef], # [POLICE OFFICER] U+E5DD -> U+1F46E
  ["eeab9b", :undef], # [WOMAN WITH BUNNY EARS] U+EADB -> U+1F46F
  ["eeaba9", :undef], # [BRIDE WITH VEIL] U+EAE9 -> U+1F470
  ["eeac93", :undef], # [WESTERN PERSON] U+EB13 -> U+1F471
  ["eeac94", :undef], # [MAN WITH GUA PI MAO] U+EB14 -> U+1F472
  ["eeac95", :undef], # [MAN WITH TURBAN] U+EB15 -> U+1F473
  ["eeac96", :undef], # [OLDER MAN] U+EB16 -> U+1F474
  ["eeac97", :undef], # [OLDER WOMAN] U+EB17 -> U+1F475
  ["eeac98", :undef], # [BABY] U+EB18 -> U+1F476
  ["eeac99", :undef], # [CONSTRUCTION WORKER] U+EB19 -> U+1F477
  ["eeac9a", :undef], # [PRINCESS] U+EB1A -> U+1F478
  ["eead84", :undef], # [JAPANESE OGRE] U+EB44 -> U+1F479
  ["eead85", :undef], # [JAPANESE GOBLIN] U+EB45 -> U+1F47A
  ["ee938b", :undef], # [GHOST] U+E4CB -> U+1F47B
  ["ee96bf", :undef], # [BABY ANGEL] U+E5BF -> U+1F47C
  ["ee948e", :undef], # [EXTRATERRESTRIAL ALIEN] U+E50E -> U+1F47D
  ["ee93ac", :undef], # [ALIEN MONSTER] U+E4EC -> U+1F47E
  ["ee93af", :undef], # [IMP] U+E4EF -> U+1F47F
  ["ee93b8", :undef], # [SKULL] U+E4F8 -> U+1F480
  ["eeac9c", :undef], # [DANCER] U+EB1C -> U+1F483
  ["eeadbe", :undef], # [SNAIL] U+EB7E -> U+1F40C
  ["eeaca2", :undef], # [SNAKE] U+EB22 -> U+1F40D
  ["eeaca3", :undef], # [CHICKEN] U+EB23 -> U+1F414
  ["eeaca4", :undef], # [BOAR] U+EB24 -> U+1F417
  ["eeaca5", :undef], # [BACTRIAN CAMEL] U+EB25 -> U+1F42B
  ["eeac9f", :undef], # [ELEPHANT] U+EB1F -> U+1F418
  ["eeaca0", :undef], # [KOALA] U+EB20 -> U+1F428
  ["ee9787", :undef], # [OCTOPUS] U+E5C7 -> U+1F419
  ["eeabac", :undef], # [SPIRAL SHELL] U+EAEC -> U+1F41A
  ["eeac9e", :undef], # [BUG] U+EB1E -> U+1F41B
  ["ee939d", :undef], # [ANT] U+E4DD -> U+1F41C
  ["eead97", :undef], # [HONEYBEE] U+EB57 -> U+1F41D
  ["eead98", :undef], # [LADY BEETLE] U+EB58 -> U+1F41E
  ["eeac9d", :undef], # [TROPICAL FISH] U+EB1D -> U+1F420
  ["ee9393", :undef], # [BLOWFISH] U+E4D3 -> U+1F421
  ["ee9794", :undef], # [TURTLE] U+E5D4 -> U+1F422
  ["ee93a0", :undef], # [BABY CHICK] U+E4E0 -> U+1F424
  ["eeadb6", :undef], # [FRONT-FACING BABY CHICK] U+EB76 -> U+1F425
  ["ee979b", :undef], # [HATCHING CHICK] U+E5DB -> U+1F423
  ["ee939c", :undef], # [PENGUIN] U+E4DC -> U+1F427
  ["ee939f", :undef], # [POODLE] U+E4DF -> U+1F429
  ["eeac9b", :undef], # [DOLPHIN] U+EB1B -> U+1F42C
  ["ee9782", :undef], # [MOUSE FACE] U+E5C2 -> U+1F42D
  ["ee9780", :undef], # [TIGER FACE] U+E5C0 -> U+1F42F
  ["ee939b", :undef], # [CAT FACE] U+E4DB -> U+1F431
  ["ee91b0", :undef], # [SPOUTING WHALE] U+E470 -> U+1F433
  ["ee9398", :undef], # [HORSE FACE] U+E4D8 -> U+1F434
  ["ee9399", :undef], # [MONKEY FACE] U+E4D9 -> U+1F435
  ["ee93a1", :undef], # [DOG FACE] U+E4E1 -> U+1F436
  ["ee939e", :undef], # [PIG FACE] U+E4DE -> U+1F437
  ["ee9781", :undef], # [BEAR FACE] U+E5C1 -> U+1F43B
  ["eeaca1", :undef], # [COW FACE] U+EB21 -> U+1F42E
  ["ee9397", :undef], # [RABBIT FACE] U+E4D7 -> U+1F430
  ["ee939a", :undef], # [FROG FACE] U+E4DA -> U+1F438
  ["ee93ae", :undef], # [PAW PRINTS] U+E4EE -> U+1F43E
  ["eeacbf", :undef], # [DRAGON FACE] U+EB3F -> U+1F432
  ["eead86", :undef], # [PANDA FACE] U+EB46 -> U+1F43C
  ["eead88", :undef], # [PIG NOSE] U+EB48 -> U+1F43D
  ["ee91b2", :undef], # [ANGRY FACE] U+E472 -> U+1F600
  ["eeada7", :undef], # [ANGUISHED FACE] U+EB67 -> U+1F601
  ["eeab8a", :undef], # [ASTONISHED FACE] U+EACA -> U+1F602
  ["ee96ae", :undef], # [DIZZY FACE] U+E5AE -> U+1F604
  ["eeab8b", :undef], # [EXASPERATED FACE] U+EACB -> U+1F605
  ["eeab89", :undef], # [EXPRESSIONLESS FACE] U+EAC9 -> U+1F606
  ["ee9784", :undef], # [FACE WITH HEART-SHAPED EYES] U+E5C4 -> U+1F607
  ["eeab81", :undef], # [FACE WITH LOOK OF TRIUMPH] U+EAC1 -> U+1F608
  ["ee93a7", :undef], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E4E7 -> U+1F609
  ["eeab8f", :undef], # [FACE THROWING A KISS] U+EACF -> U+1F60C
  ["eeab8e", :undef], # [FACE KISSING] U+EACE -> U+1F60D
  ["eeab87", :undef], # [FACE WITH MASK] U+EAC7 -> U+1F60E
  ["eeab88", :undef], # [FLUSHED FACE] U+EAC8 -> U+1F60F
  ["ee91b1", :undef], # [HAPPY FACE WITH OPEN MOUTH] U+E471 -> U+1F610
  ["eeae80", :undef], # [HAPPY FACE WITH GRIN] U+EB80 -> U+1F613
  ["eeada4", :undef], # [HAPPY AND CRYING FACE] U+EB64 -> U+1F614
  ["eeab8d", :undef], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+EACD -> U+1F615
  ["ee93bb", "e298ba"], # [WHITE SMILING FACE] U+E4FB -> U+263A
  ["eeada9", :undef], # [CRYING FACE] U+EB69 -> U+1F617
  ["ee91b3", :undef], # [LOUDLY CRYING FACE] U+E473 -> U+1F618
  ["eeab86", :undef], # [FEARFUL FACE] U+EAC6 -> U+1F619
  ["eeab82", :undef], # [PERSEVERING FACE] U+EAC2 -> U+1F61A
  ["eead9d", :undef], # [POUTING FACE] U+EB5D -> U+1F61B
  ["eeab85", :undef], # [RELIEVED FACE] U+EAC5 -> U+1F61C
  ["eeab83", :undef], # [CONFOUNDED FACE] U+EAC3 -> U+1F61D
  ["eeab80", :undef], # [PENSIVE FACE] U+EAC0 -> U+1F61E
  ["ee9785", :undef], # [FACE SCREAMING IN FEAR] U+E5C5 -> U+1F61F
  ["eeab84", :undef], # [SLEEPY FACE] U+EAC4 -> U+1F620
  ["eeaabf", :undef], # [SMIRKING FACE] U+EABF -> U+1F621
  ["ee9786", :undef], # [FACE WITH COLD SWEAT] U+E5C6 -> U+1F622
  ["ee91b4", :undef], # [TIRED FACE] U+E474 -> U+1F624
  ["ee9783", :undef], # [WINKING FACE] U+E5C3 -> U+1F625
  ["eeada1", :undef], # [CAT FACE WITH OPEN MOUTH] U+EB61 -> U+1F62B
  ["eeadbf", :undef], # [HAPPY CAT FACE WITH GRIN] U+EB7F -> U+1F62C
  ["eeada3", :undef], # [HAPPY AND CRYING CAT FACE] U+EB63 -> U+1F62D
  ["eeada0", :undef], # [CAT FACE KISSING] U+EB60 -> U+1F62E
  ["eeada5", :undef], # [CAT FACE WITH HEART-SHAPED EYES] U+EB65 -> U+1F62F
  ["eeada8", :undef], # [CRYING CAT FACE] U+EB68 -> U+1F630
  ["eead9e", :undef], # [POUTING CAT FACE] U+EB5E -> U+1F631
  ["eeadaa", :undef], # [CAT FACE WITH TIGHTLY-CLOSED LIPS] U+EB6A -> U+1F632
  ["eeada6", :undef], # [ANGUISHED CAT FACE] U+EB66 -> U+1F633
  ["eeab97", :undef], # [FACE WITH NO GOOD GESTURE] U+EAD7 -> U+1F634
  ["eeab98", :undef], # [FACE WITH OK GESTURE] U+EAD8 -> U+1F635
  ["eeab99", :undef], # [PERSON BOWING DEEPLY] U+EAD9 -> U+1F636
  ["eead90", :undef], # [SEE-NO-EVIL MONKEY] U+EB50 -> U+1F637
  ["eead91", :undef], # [SPEAK-NO-EVIL MONKEY] U+EB51 -> U+1F639
  ["eead92", :undef], # [HEAR-NO-EVIL MONKEY] U+EB52 -> U+1F638
  ["eeae85", :undef], # [PERSON RAISING ONE HAND] U+EB85 -> U+1F63A
  ["eeae86", :undef], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+EB86 -> U+1F63B
  ["eeae87", :undef], # [PERSON FROWNING] U+EB87 -> U+1F63C
  ["eeae88", :undef], # [PERSON WITH POUTING FACE] U+EB88 -> U+1F63D
  ["eeab92", :undef], # [PERSON WITH FOLDED HANDS] U+EAD2 -> U+1F63E
  ["ee92ab", :undef], # [HOUSE BUILDING] U+E4AB -> U+1F3E0
  ["eeac89", :undef], # [HOUSE WITH GARDEN] U+EB09 -> U+1F3E1
  ["ee92ad", :undef], # [OFFICE BUILDING] U+E4AD -> U+1F3E2
  ["ee979e", :undef], # [JAPANESE POST OFFICE] U+E5DE -> U+1F3E3
  ["ee979f", :undef], # [HOSPITAL] U+E5DF -> U+1F3E5
  ["ee92aa", :undef], # [BANK] U+E4AA -> U+1F3E6
  ["ee92a3", :undef], # [AUTOMATED TELLER MACHINE] U+E4A3 -> U+1F3E7
  ["eeaa81", :undef], # [HOTEL] U+EA81 -> U+1F3E8
  ["eeabb3", :undef], # [LOVE HOTEL] U+EAF3 -> U+1F3E9
  ["ee92a4", :undef], # [CONVENIENCE STORE] U+E4A4 -> U+1F3EA
  ["eeaa80", :undef], # [SCHOOL] U+EA80 -> U+1F3EB
  ["ee96bb", "e29baa"], # [CHURCH] U+E5BB -> U+26EA
  ["ee978f", "e29bb2"], # [FOUNTAIN] U+E5CF -> U+26F2
  ["eeabb6", :undef], # [DEPARTMENT STORE] U+EAF6 -> U+1F3EC
  ["eeabb7", :undef], # [JAPANESE CASTLE] U+EAF7 -> U+1F3EF
  ["eeabb8", :undef], # [EUROPEAN CASTLE] U+EAF8 -> U+1F3F0
  ["eeabb9", :undef], # [FACTORY] U+EAF9 -> U+1F3ED
  ["ee92a9", "e29a93"], # [ANCHOR] U+E4A9 -> U+2693
  ["ee92bd", :undef], # [IZAKAYA LANTERN] U+E4BD -> U+1F3EE
  ["ee96bd", :undef], # [MOUNT FUJI] U+E5BD -> U+1F5FB
  ["ee9380", :undef], # [TOKYO TOWER] U+E4C0 -> U+1F5FC
  ["ee95b2", :undef], # [SILHOUETTE OF JAPAN] U+E572 -> U+1F5FE
  ["eeadac", :undef], # [MOYAI] U+EB6C -> U+1F5FF
  ["ee96b7", :undef], # [MANS SHOE] U+E5B7 -> U+1F45E
  ["eeacab", :undef], # [ATHLETIC SHOE] U+EB2B -> U+1F45F
  ["ee949a", :undef], # [HIGH-HEELED SHOE] U+E51A -> U+1F460
  ["eeaa9f", :undef], # [WOMANS BOOTS] U+EA9F -> U+1F462
  ["eeacaa", :undef], # [FOOTPRINTS] U+EB2A -> U+1F463
  ["ee93be", :undef], # [EYEGLASSES] U+E4FE -> U+1F453
  ["ee96b6", :undef], # [T-SHIRT] U+E5B6 -> U+1F455
  ["eeadb7", :undef], # [JEANS] U+EB77 -> U+1F456
  ["ee9789", :undef], # [CROWN] U+E5C9 -> U+1F451
  ["eeaa93", :undef], # [NECKTIE] U+EA93 -> U+1F454
  ["eeaa9e", :undef], # [WOMANS HAT] U+EA9E -> U+1F452
  ["eeadab", :undef], # [DRESS] U+EB6B -> U+1F457
  ["eeaaa3", :undef], # [KIMONO] U+EAA3 -> U+1F458
  ["eeaaa4", :undef], # [BIKINI] U+EAA4 -> U+1F459
  ["ee948d", :undef], # [WOMANS CLOTHES] U+E50D -> U+1F45A
  ["ee9484", :undef], # [PURSE] U+E504 -> U+1F45B
  ["ee929c", :undef], # [HANDBAG] U+E49C -> U+1F45C
  ["ee9387", :undef], # [MONEY BAG] U+E4C7 -> U+1F4B0
  ["ee979c", :undef], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+E5DC -> U+1F4B9
  ["ee95b9", :undef], # [HEAVY DOLLAR SIGN] U+E579 -> U+1F4B2
  ["ee95bc", :undef], # [CREDIT CARD] U+E57C -> U+1F4B3
  ["ee95bd", :undef], # [BANKNOTE WITH YEN SIGN] U+E57D -> U+1F4B4
  ["ee9685", :undef], # [BANKNOTE WITH DOLLAR SIGN] U+E585 -> U+1F4B5
  ["eead9b", :undef], # [MONEY WITH WINGS] U+EB5B -> U+1F4B8
  ["eeac91", "f09f87a8f09f87b3"], # [REGIONAL INDICATOR SYMBOL LETTERS CN] U+EB11 -> U+1F1E8 U+1F1F3
  ["eeac8e", "f09f87a9f09f87aa"], # [REGIONAL INDICATOR SYMBOL LETTERS DE] U+EB0E -> U+1F1E9 U+1F1EA
  ["ee9795", "f09f87aaf09f87b8"], # [REGIONAL INDICATOR SYMBOL LETTERS ES] U+E5D5 -> U+1F1EA U+1F1F8
  ["eeabba", "f09f87abf09f87b7"], # [REGIONAL INDICATOR SYMBOL LETTERS FR] U+EAFA -> U+1F1EB U+1F1F7
  ["eeac90", "f09f87baf09f87b0"], # [REGIONAL INDICATOR SYMBOL LETTERS GB] U+EB10 -> U+1F1FA U+1F1F0
  ["eeac8f", "f09f87aef09f87b9"], # [REGIONAL INDICATOR SYMBOL LETTERS IT] U+EB0F -> U+1F1EE U+1F1F9
  ["ee938c", "f09f87aff09f87b5"], # [REGIONAL INDICATOR SYMBOL LETTERS JP] U+E4CC -> U+1F1EF U+1F1F5
  ["eeac92", "f09f87b0f09f87b7"], # [REGIONAL INDICATOR SYMBOL LETTERS KR] U+EB12 -> U+1F1F0 U+1F1F7
  ["ee9796", "f09f87b7f09f87ba"], # [REGIONAL INDICATOR SYMBOL LETTERS RU] U+E5D6 -> U+1F1F7 U+1F1FA
  ["ee95b3", "f09f87baf09f87b8"], # [REGIONAL INDICATOR SYMBOL LETTERS US] U+E573 -> U+1F1FA U+1F1F8
  ["ee91bb", :undef], # [FIRE] U+E47B -> U+1F525
  ["ee9683", :undef], # [ELECTRIC TORCH] U+E583 -> U+1F526
  ["ee9687", :undef], # [WRENCH] U+E587 -> U+1F527
  ["ee978b", :undef], # [HAMMER] U+E5CB -> U+1F528
  ["ee9681", :undef], # [NUT AND BOLT] U+E581 -> U+1F529
  ["ee95bf", :undef], # [HOCHO] U+E57F -> U+1F52A
  ["ee948a", :undef], # [PISTOL] U+E50A -> U+1F52B
  ["eeaa8f", :undef], # [CRYSTAL BALL] U+EA8F -> U+1F52E
  ["ee9280", :undef], # [JAPANESE SYMBOL FOR BEGINNER] U+E480 -> U+1F530
  ["ee9490", :undef], # [SYRINGE] U+E510 -> U+1F489
  ["eeaa9a", :undef], # [PILL] U+EA9A -> U+1F48A
  ["eeaca6", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+EB26 -> U+1F170
  ["eeaca7", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+EB27 -> U+1F171
  ["eeaca9", :undef], # [NEGATIVE SQUARED AB] U+EB29 -> U+1F18E
  ["eeaca8", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+EB28 -> U+1F17E
  ["ee969f", :undef], # [RIBBON] U+E59F -> U+1F380
  ["ee938f", :undef], # [WRAPPED PRESENT] U+E4CF -> U+1F381
  ["ee96a0", :undef], # [BIRTHDAY CAKE] U+E5A0 -> U+1F382
  ["ee9389", :undef], # [CHRISTMAS TREE] U+E4C9 -> U+1F384
  ["eeabb0", :undef], # [FATHER CHRISTMAS] U+EAF0 -> U+1F385
  ["ee9799", :undef], # [CROSSED FLAGS] U+E5D9 -> U+1F38C
  ["ee978c", :undef], # [FIREWORKS] U+E5CC -> U+1F386
  ["eeaa9b", :undef], # [BALLOON] U+EA9B -> U+1F388
  ["eeaa9c", :undef], # [PARTY POPPER] U+EA9C -> U+1F389
  ["eeaba3", :undef], # [PINE DECORATION] U+EAE3 -> U+1F38D
  ["eeaba4", :undef], # [JAPANESE DOLLS] U+EAE4 -> U+1F38E
  ["eeaba5", :undef], # [GRADUATION CAP] U+EAE5 -> U+1F393
  ["eeaba6", :undef], # [SCHOOL SATCHEL] U+EAE6 -> U+1F392
  ["eeaba7", :undef], # [CARP STREAMER] U+EAE7 -> U+1F38F
  ["eeabab", :undef], # [FIREWORK SPARKLER] U+EAEB -> U+1F387
  ["eeabad", :undef], # [WIND CHIME] U+EAED -> U+1F390
  ["eeabae", :undef], # [JACK-O-LANTERN] U+EAEE -> U+1F383
  ["ee91af", :undef], # [CONFETTI BALL] U+E46F -> U+1F38A
  ["eeacbd", :undef], # [TANABATA TREE] U+EB3D -> U+1F38B
  ["eeabaf", :undef], # [MOON VIEWING CEREMONY] U+EAEF -> U+1F391
  ["ee969b", :undef], # [PAGER] U+E59B -> U+1F4DF
  ["ee9696", "e2988e"], # [BLACK TELEPHONE] U+E596 -> U+260E
  ["ee949e", :undef], # [TELEPHONE RECEIVER] U+E51E -> U+1F4DE
  ["ee9688", :undef], # [MOBILE PHONE] U+E588 -> U+1F4F1
  ["eeac88", :undef], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+EB08 -> U+1F4F2
  ["eeaa92", :undef], # [MEMO] U+EA92 -> U+1F4DD
  ["ee94a0", :undef], # [FAX MACHINE] U+E520 -> U+1F4E0
  ["ee94a1", "e29c89"], # [ENVELOPE] U+E521 -> U+2709
  ["ee9691", :undef], # [INCOMING ENVELOPE] U+E591 -> U+1F4E8
  ["eeada2", :undef], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+EB62 -> U+1F4E9
  ["ee949b", :undef], # [CLOSED MAILBOX WITH LOWERED FLAG] U+E51B -> U+1F4EA
  ["eeac8a", :undef], # [CLOSED MAILBOX WITH RAISED FLAG] U+EB0A -> U+1F4EB
  ["ee968b", :undef], # [NEWSPAPER] U+E58B -> U+1F4F0
  ["ee92a8", :undef], # [SATELLITE ANTENNA] U+E4A8 -> U+1F4E1
  ["ee9692", :undef], # [OUTBOX TRAY] U+E592 -> U+1F4E4
  ["ee9693", :undef], # [INBOX TRAY] U+E593 -> U+1F4E5
  ["ee949f", :undef], # [PACKAGE] U+E51F -> U+1F4E6
  ["eeadb1", :undef], # [E-MAIL SYMBOL] U+EB71 -> U+1F4E7
  ["eeabbd", :undef], # [INPUT SYMBOL FOR LATIN CAPITAL LETTERS] U+EAFD -> U+1F520
  ["eeabbe", :undef], # [INPUT SYMBOL FOR LATIN SMALL LETTERS] U+EAFE -> U+1F521
  ["eeabbf", :undef], # [INPUT SYMBOL FOR NUMBERS] U+EAFF -> U+1F522
  ["eeac80", :undef], # [INPUT SYMBOL FOR SYMBOLS] U+EB00 -> U+1F523
  ["eead95", :undef], # [INPUT SYMBOL FOR LATIN LETTERS] U+EB55 -> U+1F524
  ["eeac83", "e29c92"], # [BLACK NIB] U+EB03 -> U+2712
  ["ee96b8", :undef], # [PERSONAL COMPUTER] U+E5B8 -> U+1F4BB
  ["ee92a1", "e29c8f"], # [PENCIL] U+E4A1 -> U+270F
  ["ee92a0", :undef], # [PAPERCLIP] U+E4A0 -> U+1F4CE
  ["ee978e", :undef], # [BRIEFCASE] U+E5CE -> U+1F4BC
  ["ee9682", :undef], # [MINIDISC] U+E582 -> U+1F4BD
  ["ee95a2", :undef], # [FLOPPY DISK] U+E562 -> U+1F4BE
  ["ee948c", :undef], # [OPTICAL DISC] U+E50C -> U+1F4BF
  ["ee9496", "e29c82"], # [BLACK SCISSORS] U+E516 -> U+2702
  ["ee95a0", :undef], # [ROUND PUSHPIN] U+E560 -> U+1F4CD
  ["ee95a1", :undef], # [PAGE WITH CURL] U+E561 -> U+1F4C3
  ["ee95a9", :undef], # [PAGE FACING UP] U+E569 -> U+1F4C4
  ["ee95a3", :undef], # [CALENDAR] U+E563 -> U+1F4C5
  ["ee968f", :undef], # [FILE FOLDER] U+E58F -> U+1F4C1
  ["ee9690", :undef], # [OPEN FILE FOLDER] U+E590 -> U+1F4C2
  ["ee95ab", :undef], # [NOTEBOOK] U+E56B -> U+1F4D3
  ["ee929f", :undef], # [OPEN BOOK] U+E49F -> U+1F4D6
  ["ee929d", :undef], # [NOTEBOOK WITH DECORATIVE COVER] U+E49D -> U+1F4D4
  ["ee95a8", :undef], # [CLOSED BOOK] U+E568 -> U+1F4D5
  ["ee95a5", :undef], # [GREEN BOOK] U+E565 -> U+1F4D7
  ["ee95a6", :undef], # [BLUE BOOK] U+E566 -> U+1F4D8
  ["ee95a7", :undef], # [ORANGE BOOK] U+E567 -> U+1F4D9
  ["ee95af", :undef], # [BOOKS] U+E56F -> U+1F4DA
  ["ee949d", :undef], # [NAME BADGE] U+E51D -> U+1F4DB
  ["ee959f", :undef], # [SCROLL] U+E55F -> U+1F4DC
  ["ee95a4", :undef], # [CLIPBOARD] U+E564 -> U+1F4CB
  ["ee95aa", :undef], # [TEAR-OFF CALENDAR] U+E56A -> U+1F4C6
  ["ee95b4", :undef], # [BAR CHART] U+E574 -> U+1F4CA
  ["ee95b5", :undef], # [CHART WITH UPWARDS TREND] U+E575 -> U+1F4C8
  ["ee95b6", :undef], # [CHART WITH DOWNWARDS TREND] U+E576 -> U+1F4C9
  ["ee95ac", :undef], # [CARD INDEX] U+E56C -> U+1F4C7
  ["ee95ad", :undef], # [PUSHPIN] U+E56D -> U+1F4CC
  ["ee95ae", :undef], # [LEDGER] U+E56E -> U+1F4D2
  ["ee95b0", :undef], # [STRAIGHT RULER] U+E570 -> U+1F4CF
  ["ee92a2", :undef], # [TRIANGULAR RULER] U+E4A2 -> U+1F4D0
  ["eeac8b", :undef], # [BOOKMARK TABS] U+EB0B -> U+1F4D1
  ["ee92ba", "e29abe"], # [BASEBALL] U+E4BA -> U+26BE
  ["ee9699", "e29bb3"], # [FLAG IN HOLE] U+E599 -> U+26F3
  ["ee92b7", :undef], # [TENNIS RACQUET AND BALL] U+E4B7 -> U+1F3BE
  ["ee92b6", "e29abd"], # [SOCCER BALL] U+E4B6 -> U+26BD
  ["eeaaac", :undef], # [SKI AND SKI BOOT] U+EAAC -> U+1F3BF
  ["ee969a", :undef], # [BASKETBALL AND HOOP] U+E59A -> U+1F3C0
  ["ee92b9", :undef], # [CHEQUERED FLAG] U+E4B9 -> U+1F3C1
  ["ee92b8", :undef], # [SNOWBOARDER] U+E4B8 -> U+1F3C2
  ["ee91ab", :undef], # [RUNNER] U+E46B -> U+1F3C3
  ["eead81", :undef], # [SURFER] U+EB41 -> U+1F3C4
  ["ee9793", :undef], # [TROPHY] U+E5D3 -> U+1F3C6
  ["ee92bb", :undef], # [AMERICAN FOOTBALL] U+E4BB -> U+1F3C8
  ["eeab9e", :undef], # [SWIMMER] U+EADE -> U+1F3CA
  ["ee92b5", :undef], # [TRAIN] U+E4B5 -> U+1F686
  ["ee96bc", :undef], # [METRO] U+E5BC -> U+1F687
  ["ee92b0", :undef], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+E4B0 -> U+1F685
  ["ee92b1", :undef], # [AUTOMOBILE] U+E4B1 -> U+1F697
  ["ee92af", :undef], # [ONCOMING BUS] U+E4AF -> U+1F68D
  ["ee92a7", :undef], # [BUS STOP] U+E4A7 -> U+1F68F
  ["eeaa82", :undef], # [SHIP] U+EA82 -> U+1F6A2
  ["ee92b3", "e29c88"], # [AIRPLANE] U+E4B3 -> U+2708
  ["ee92b4", "e29bb5"], # [SAILBOAT] U+E4B4 -> U+26F5
  ["eeadad", :undef], # [STATION] U+EB6D -> U+1F689
  ["ee9788", :undef], # [ROCKET] U+E5C8 -> U+1F680
  ["ee92b2", :undef], # [DELIVERY TRUCK] U+E4B2 -> U+1F69A
  ["eeab9f", :undef], # [FIRE ENGINE] U+EADF -> U+1F692
  ["eeaba0", :undef], # [AMBULANCE] U+EAE0 -> U+1F691
  ["eeaba1", :undef], # [POLICE CAR] U+EAE1 -> U+1F693
  ["ee95b1", "e29bbd"], # [FUEL PUMP] U+E571 -> U+26FD
  ["ee92a6", "f09f85bf"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E4A6 -> U+1F17F
  ["ee91aa", :undef], # [HORIZONTAL TRAFFIC LIGHT] U+E46A -> U+1F6A5
  ["ee9797", :undef], # [CONSTRUCTION SIGN] U+E5D7 -> U+1F6A7
  ["eeadb3", :undef], # [POLICE CARS REVOLVING LIGHT] U+EB73 -> U+1F6A8
  ["ee92bc", "e299a8"], # [HOT SPRINGS] U+E4BC -> U+2668
  ["ee9790", "e29bba"], # [TENT] U+E5D0 -> U+26FA
  ["ee91ad", :undef], # [FERRIS WHEEL] U+E46D -> U+1F3A1
  ["eeaba2", :undef], # [ROLLER COASTER] U+EAE2 -> U+1F3A2
  ["eead82", :undef], # [FISHING POLE AND FISH] U+EB42 -> U+1F3A3
  ["ee9483", :undef], # [MICROPHONE] U+E503 -> U+1F3A4
  ["ee9497", :undef], # [MOVIE CAMERA] U+E517 -> U+1F3A5
  ["ee9488", :undef], # [HEADPHONE] U+E508 -> U+1F3A7
  ["ee969c", :undef], # [ARTIST PALETTE] U+E59C -> U+1F3A8
  ["eeabb5", :undef], # [TOP HAT] U+EAF5 -> U+1F3A9
  ["ee969e", :undef], # [CIRCUS TENT] U+E59E -> U+1F3AA
  ["ee929e", :undef], # [TICKET] U+E49E -> U+1F3AB
  ["ee92be", :undef], # [CLAPPER BOARD] U+E4BE -> U+1F3AC
  ["ee969d", :undef], # [PERFORMING ARTS] U+E59D -> U+1F3AD
  ["ee9386", :undef], # [VIDEO GAME] U+E4C6 -> U+1F3AE
  ["ee9791", "f09f8084"], # [MAHJONG TILE RED DRAGON] U+E5D1 -> U+1F004
  ["ee9385", :undef], # [DIRECT HIT] U+E4C5 -> U+1F3AF
  ["ee91ae", :undef], # [SLOT MACHINE] U+E46E -> U+1F3B0
  ["eeab9d", :undef], # [BILLIARDS] U+EADD -> U+1F3B1
  ["ee9388", :undef], # [GAME DIE] U+E4C8 -> U+1F3B2
  ["eead83", :undef], # [BOWLING] U+EB43 -> U+1F3B3
  ["eeadae", :undef], # [FLOWER PLAYING CARDS] U+EB6E -> U+1F3B4
  ["eeadaf", :undef], # [PLAYING CARD BLACK JOKER] U+EB6F -> U+1F0CF
  ["ee96be", :undef], # [MUSICAL NOTE] U+E5BE -> U+1F3B5
  ["ee9485", :undef], # [MULTIPLE MUSICAL NOTES] U+E505 -> U+1F3B6
  ["ee9486", :undef], # [GUITAR] U+E506 -> U+1F3B8
  ["eead80", :undef], # [MUSICAL KEYBOARD] U+EB40 -> U+1F3B9
  ["eeab9c", :undef], # [TRUMPET] U+EADC -> U+1F3BA
  ["ee9487", :undef], # [VIOLIN] U+E507 -> U+1F3BB
  ["eeab8c", :undef], # [MUSICAL SCORE] U+EACC -> U+1F3BC
  ["ee9495", :undef], # [CAMERA] U+E515 -> U+1F4F7
  ["ee95be", :undef], # [VIDEO CAMERA] U+E57E -> U+1F4F9
  ["ee9482", :undef], # [TELEVISION] U+E502 -> U+1F4FA
  ["ee96b9", :undef], # [RADIO] U+E5B9 -> U+1F4FB
  ["ee9680", :undef], # [VIDEOCASSETTE] U+E580 -> U+1F4FC
  ["ee93ab", :undef], # [KISS MARK] U+E4EB -> U+1F48B
  ["eeadb8", :undef], # [LOVE LETTER] U+EB78 -> U+1F48C
  ["ee9494", :undef], # [RING] U+E514 -> U+1F48D
  ["ee978a", :undef], # [KISS] U+E5CA -> U+1F48F
  ["eeaa95", :undef], # [BOUQUET] U+EA95 -> U+1F490
  ["eeab9a", :undef], # [COUPLE WITH HEART] U+EADA -> U+1F491
  ["eeaa83", :undef], # [NO ONE UNDER EIGHTEEN SYMBOL] U+EA83 -> U+1F51E
  ["ee9598", "c2a9"], # [COPYRIGHT SIGN] U+E558 -> U+A9
  ["ee9599", "c2ae"], # [REGISTERED SIGN] U+E559 -> U+AE
  ["ee958e", "e284a2"], # [TRADE MARK SIGN] U+E54E -> U+2122
  ["ee94b3", "e284b9"], # [INFORMATION SOURCE] U+E533 -> U+2139
  ["eeae84", "23e283a3"], # [HASH KEY] U+EB84 -> U+23 U+20E3
  ["ee94a2", "31e283a3"], # [KEYCAP 1] U+E522 -> U+31 U+20E3
  ["ee94a3", "32e283a3"], # [KEYCAP 2] U+E523 -> U+32 U+20E3
  ["ee94a4", "33e283a3"], # [KEYCAP 3] U+E524 -> U+33 U+20E3
  ["ee94a5", "34e283a3"], # [KEYCAP 4] U+E525 -> U+34 U+20E3
  ["ee94a6", "35e283a3"], # [KEYCAP 5] U+E526 -> U+35 U+20E3
  ["ee94a7", "36e283a3"], # [KEYCAP 6] U+E527 -> U+36 U+20E3
  ["ee94a8", "37e283a3"], # [KEYCAP 7] U+E528 -> U+37 U+20E3
  ["ee94a9", "38e283a3"], # [KEYCAP 8] U+E529 -> U+38 U+20E3
  ["ee94aa", "39e283a3"], # [KEYCAP 9] U+E52A -> U+39 U+20E3
  ["ee96ac", "30e283a3"], # [KEYCAP 0] U+E5AC -> U+30 U+20E3
  ["ee94ab", :undef], # [KEYCAP TEN] U+E52B -> U+1F51F
  ["eeaa84", :undef], # [ANTENNA WITH BARS] U+EA84 -> U+1F4F6
  ["eeaa90", :undef], # [VIBRATION MODE] U+EA90 -> U+1F4F3
  ["eeaa91", :undef], # [MOBILE PHONE OFF] U+EA91 -> U+1F4F4
  ["ee9396", :undef], # [HAMBURGER] U+E4D6 -> U+1F354
  ["ee9395", :undef], # [RICE BALL] U+E4D5 -> U+1F359
  ["ee9390", :undef], # [SHORTCAKE] U+E4D0 -> U+1F370
  ["ee96b4", :undef], # [STEAMING BOWL] U+E5B4 -> U+1F35C
  ["eeaaaf", :undef], # [BREAD] U+EAAF -> U+1F35E
  ["ee9391", :undef], # [COOKING] U+E4D1 -> U+1F373
  ["eeaab0", :undef], # [SOFT ICE CREAM] U+EAB0 -> U+1F366
  ["eeaab1", :undef], # [FRENCH FRIES] U+EAB1 -> U+1F35F
  ["eeaab2", :undef], # [DANGO] U+EAB2 -> U+1F361
  ["eeaab3", :undef], # [RICE CRACKER] U+EAB3 -> U+1F358
  ["eeaab4", :undef], # [COOKED RICE] U+EAB4 -> U+1F35A
  ["eeaab5", :undef], # [SPAGHETTI] U+EAB5 -> U+1F35D
  ["eeaab6", :undef], # [CURRY AND RICE] U+EAB6 -> U+1F35B
  ["eeaab7", :undef], # [ODEN] U+EAB7 -> U+1F362
  ["eeaab8", :undef], # [SUSHI] U+EAB8 -> U+1F363
  ["eeaabd", :undef], # [BENTO BOX] U+EABD -> U+1F371
  ["eeaabe", :undef], # [POT OF FOOD] U+EABE -> U+1F372
  ["eeabaa", :undef], # [SHAVED ICE] U+EAEA -> U+1F367
  ["ee9384", :undef], # [MEAT ON BONE] U+E4C4 -> U+1F356
  ["ee93ad", :undef], # [FISH CAKE WITH SWIRL DESIGN] U+E4ED -> U+1F365
  ["eeacba", :undef], # [ROASTED SWEET POTATO] U+EB3A -> U+1F360
  ["eeacbb", :undef], # [SLICE OF PIZZA] U+EB3B -> U+1F355
  ["eeacbc", :undef], # [POULTRY LEG] U+EB3C -> U+1F357
  ["eead8a", :undef], # [ICE CREAM] U+EB4A -> U+1F368
  ["eead8b", :undef], # [DOUGHNUT] U+EB4B -> U+1F369
  ["eead8c", :undef], # [COOKIE] U+EB4C -> U+1F36A
  ["eead8d", :undef], # [CHOCOLATE BAR] U+EB4D -> U+1F36B
  ["eead8e", :undef], # [CANDY] U+EB4E -> U+1F36C
  ["eead8f", :undef], # [LOLLIPOP] U+EB4F -> U+1F36D
  ["eead96", :undef], # [CUSTARD] U+EB56 -> U+1F36E
  ["eead99", :undef], # [HONEY POT] U+EB59 -> U+1F36F
  ["eeadb0", :undef], # [FRIED SHRIMP] U+EB70 -> U+1F364
  ["ee92ac", :undef], # [FORK AND KNIFE] U+E4AC -> U+1F374
  ["ee9697", "e29895"], # [HOT BEVERAGE] U+E597 -> U+2615
  ["ee9382", :undef], # [COCKTAIL GLASS] U+E4C2 -> U+1F378
  ["ee9383", :undef], # [BEER MUG] U+E4C3 -> U+1F37A
  ["eeaaae", :undef], # [TEACUP WITHOUT HANDLE] U+EAAE -> U+1F375
  ["eeaa97", :undef], # [SAKE BOTTLE AND CUP] U+EA97 -> U+1F376
  ["ee9381", :undef], # [WINE GLASS] U+E4C1 -> U+1F377
  ["eeaa98", :undef], # [CLINKING BEER MUGS] U+EA98 -> U+1F37B
  ["eeacbe", :undef], # [TROPICAL DRINK] U+EB3E -> U+1F379
  ["ee9595", "e28697"], # [NORTH EAST ARROW] U+E555 -> U+2197
  ["ee958d", "e28698"], # [SOUTH EAST ARROW] U+E54D -> U+2198
  ["ee958c", "e28696"], # [NORTH WEST ARROW] U+E54C -> U+2196
  ["ee9596", "e28699"], # [SOUTH WEST ARROW] U+E556 -> U+2199
  ["eeacad", "e2a4b4"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+EB2D -> U+2934
  ["eeacae", "e2a4b5"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+EB2E -> U+2935
  ["eeadba", "e28694"], # [LEFT RIGHT ARROW] U+EB7A -> U+2194
  ["eeadbb", "e28695"], # [UP DOWN ARROW] U+EB7B -> U+2195
  ["ee94bf", "e2ac86"], # [UPWARDS BLACK ARROW] U+E53F -> U+2B06
  ["ee9580", "e2ac87"], # [DOWNWARDS BLACK ARROW] U+E540 -> U+2B07
  ["ee9592", "e29ea1"], # [BLACK RIGHTWARDS ARROW] U+E552 -> U+27A1
  ["ee9593", "e2ac85"], # [LEFTWARDS BLACK ARROW] U+E553 -> U+2B05
  ["ee94ae", "e296b6"], # [BLACK RIGHT-POINTING TRIANGLE] U+E52E -> U+25B6
  ["ee94ad", "e29780"], # [BLACK LEFT-POINTING TRIANGLE] U+E52D -> U+25C0
  ["ee94b0", :undef], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+E530 -> U+23E9
  ["ee94af", :undef], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+E52F -> U+23EA
  ["ee9585", :undef], # [BLACK UP-POINTING DOUBLE TRIANGLE] U+E545 -> U+23EB
  ["ee9584", :undef], # [BLACK DOWN-POINTING DOUBLE TRIANGLE] U+E544 -> U+23EC
  ["ee959a", :undef], # [UP-POINTING RED TRIANGLE] U+E55A -> U+1F53A
  ["ee959b", :undef], # [DOWN-POINTING RED TRIANGLE] U+E55B -> U+1F53B
  ["ee9583", :undef], # [UP-POINTING SMALL RED TRIANGLE] U+E543 -> U+1F53C
  ["ee9582", :undef], # [DOWN-POINTING SMALL RED TRIANGLE] U+E542 -> U+1F53D
  ["eeaaad", "e2ad95"], # [HEAVY LARGE CIRCLE] U+EAAD -> U+2B55
  ["ee9590", :undef], # [CROSS MARK] U+E550 -> U+274C
  ["ee9591", :undef], # [NEGATIVE SQUARED CROSS MARK] U+E551 -> U+274E
  ["ee9282", "e29da2"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E482 -> U+2762
  ["eeacaf", "e28189"], # [EXCLAMATION QUESTION MARK] U+EB2F -> U+2049
  ["eeacb0", "e280bc"], # [DOUBLE EXCLAMATION MARK] U+EB30 -> U+203C
  ["ee9283", :undef], # [BLACK QUESTION MARK ORNAMENT] U+E483 -> U+2753
  ["eeacb1", :undef], # [CURLY LOOP] U+EB31 -> U+27B0
  ["ee9695", "e29da4"], # [HEAVY BLACK HEART] U+E595 -> U+2764
  ["eeadb5", :undef], # [BEATING HEART] U+EB75 -> U+1F493
  ["ee91b7", :undef], # [BROKEN HEART] U+E477 -> U+1F494
  ["ee91b8", :undef], # [TWO HEARTS] U+E478 -> U+1F495
  ["eeaaa6", :undef], # [SPARKLING HEART] U+EAA6 -> U+1F496
  ["ee93aa", :undef], # [HEART WITH ARROW] U+E4EA -> U+1F498
  ["eeaaa7", :undef], # [BLUE HEART] U+EAA7 -> U+1F499
  ["eeaaa8", :undef], # [GREEN HEART] U+EAA8 -> U+1F49A
  ["eeaaa9", :undef], # [YELLOW HEART] U+EAA9 -> U+1F49B
  ["eeaaaa", :undef], # [PURPLE HEART] U+EAAA -> U+1F49C
  ["eead94", :undef], # [HEART WITH RIBBON] U+EB54 -> U+1F49D
  ["ee96af", :undef], # [REVOLVING HEARTS] U+E5AF -> U+1F49E
  ["eeaaa5", "e299a5"], # [BLACK HEART SUIT] U+EAA5 -> U+2665
  ["ee96a1", "e299a0"], # [BLACK SPADE SUIT] U+E5A1 -> U+2660
  ["ee96a2", "e299a6"], # [BLACK DIAMOND SUIT] U+E5A2 -> U+2666
  ["ee96a3", "e299a3"], # [BLACK CLUB SUIT] U+E5A3 -> U+2663
  ["ee91bd", :undef], # [SMOKING SYMBOL] U+E47D -> U+1F6AC
  ["ee91be", :undef], # [NO SMOKING SYMBOL] U+E47E -> U+1F6AD
  ["ee91bf", "e299bf"], # [WHEELCHAIR SYMBOL] U+E47F -> U+267F
  ["eeacac", :undef], # [TRIANGULAR FLAG ON POST] U+EB2C -> U+1F6A9
  ["ee9281", "e29aa0"], # [WARNING SIGN] U+E481 -> U+26A0
  ["ee9284", "e29b94"], # [NO ENTRY] U+E484 -> U+26D4
  ["eeadb9", "e299bb"], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+EB79 -> U+267B
  ["ee92ae", :undef], # [BICYCLE] U+E4AE -> U+1F6B2
  ["eeadb2", :undef], # [PEDESTRIAN] U+EB72 -> U+1F6B6
  ["ee9798", :undef], # [BATH] U+E5D8 -> U+1F6C0
  ["ee92a5", :undef], # [RESTROOM] U+E4A5 -> U+1F6BB
  ["ee9581", :undef], # [NO ENTRY SIGN] U+E541 -> U+1F6AB
  ["ee9597", "e29c94"], # [HEAVY CHECK MARK] U+E557 -> U+2714
  ["ee96ab", :undef], # [SQUARED CL] U+E5AB -> U+1F191
  ["eeaa85", :undef], # [SQUARED COOL] U+EA85 -> U+1F192
  ["ee95b8", :undef], # [SQUARED FREE] U+E578 -> U+1F193
  ["eeaa88", :undef], # [SQUARED ID] U+EA88 -> U+1F194
  ["ee96b5", :undef], # [SQUARED NEW] U+E5B5 -> U+1F195
  ["ee96ad", :undef], # [SQUARED OK] U+E5AD -> U+1F197
  ["ee93a8", :undef], # [SQUARED SOS] U+E4E8 -> U+1F198
  ["ee948f", :undef], # [SQUARED UP WITH EXCLAMATION MARK] U+E50F -> U+1F199
  ["ee9792", :undef], # [SQUARED VS] U+E5D2 -> U+1F19A
  ["eeaa87", :undef], # [SQUARED KATAKANA SA] U+EA87 -> U+1F202
  ["eeaa8a", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+EA8A -> U+1F233
  ["eeaa89", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+EA89 -> U+1F235
  ["eeaa86", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+EA86 -> U+1F239
  ["eeaa8b", "f09f88af"], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+EA8B -> U+1F22F
  ["eeaa8c", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+EA8C -> U+1F23A
  ["ee93b1", "e38a99"], # [CIRCLED IDEOGRAPH SECRET] U+E4F1 -> U+3299
  ["eeaa99", "e38a97"], # [CIRCLED IDEOGRAPH CONGRATULATION] U+EA99 -> U+3297
  ["ee93b7", :undef], # [CIRCLED IDEOGRAPH ADVANTAGE] U+E4F7 -> U+1F250
  ["eeac81", :undef], # [CIRCLED IDEOGRAPH ACCEPT] U+EB01 -> U+1F251
  ["ee94bc", :undef], # [HEAVY PLUS SIGN] U+E53C -> U+2795
  ["ee94bd", :undef], # [HEAVY MINUS SIGN] U+E53D -> U+2796
  ["ee958f", "e29c96"], # [HEAVY MULTIPLICATION X] U+E54F -> U+2716
  ["ee9594", :undef], # [HEAVY DIVISION SIGN] U+E554 -> U+2797
  ["ee91b6", :undef], # [ELECTRIC LIGHT BULB] U+E476 -> U+1F4A1
  ["ee93a5", :undef], # [ANGER SYMBOL] U+E4E5 -> U+1F4A2
  ["ee91ba", :undef], # [BOMB] U+E47A -> U+1F4A3
  ["ee91b5", :undef], # [SLEEPING SYMBOL] U+E475 -> U+1F4A4
  ["ee96b0", :undef], # [COLLISION SYMBOL] U+E5B0 -> U+1F4A5
  ["ee96b1", :undef], # [SPLASHING SWEAT SYMBOL] U+E5B1 -> U+1F4A6
  ["ee93a6", :undef], # [DROP OF WATER] U+E4E6 -> U+1F4A7
  ["ee93b4", :undef], # [DASH SYMBOL] U+E4F4 -> U+1F4A8
  ["ee93b5", :undef], # [PILE OF POO] U+E4F5 -> U+1F4A9
  ["ee93a9", :undef], # [FLEXED BICEPS] U+E4E9 -> U+1F4AA
  ["eead9c", :undef], # [DIZZY SYMBOL] U+EB5C -> U+1F4AB
  ["ee93bd", :undef], # [SPEECH BALLOON] U+E4FD -> U+1F4AC
  ["eeaaab", :undef], # [SPARKLES] U+EAAB -> U+2728
  ["ee91b9", "e29cb4"], # [EIGHT POINTED BLACK STAR] U+E479 -> U+2734
  ["ee94be", "e29cb3"], # [EIGHT SPOKED ASTERISK] U+E53E -> U+2733
  ["ee94ba", "e29aaa"], # [MEDIUM WHITE CIRCLE] U+E53A -> U+26AA
  ["ee94bb", "e29aab"], # [MEDIUM BLACK CIRCLE] U+E53B -> U+26AB
  ["ee958a", :undef], # [LARGE RED CIRCLE] U+E54A -> U+1F534
  ["ee958b", :undef], # [LARGE BLUE CIRCLE] U+E54B -> U+1F535
  ["ee928b", "e2ad90"], # [WHITE MEDIUM STAR] U+E48B -> U+2B50
  ["ee9588", "e2ac9c"], # [WHITE LARGE SQUARE] U+E548 -> U+2B1C
  ["ee9589", "e2ac9b"], # [BLACK LARGE SQUARE] U+E549 -> U+2B1B
  ["ee94b1", "e296ab"], # [WHITE SMALL SQUARE] U+E531 -> U+25AB
  ["ee94b2", "e296aa"], # [BLACK SMALL SQUARE] U+E532 -> U+25AA
  ["ee94b4", "e297bd"], # [WHITE MEDIUM SMALL SQUARE] U+E534 -> U+25FD
  ["ee94b5", "e297be"], # [BLACK MEDIUM SMALL SQUARE] U+E535 -> U+25FE
  ["ee94b8", "e297bb"], # [WHITE MEDIUM SQUARE] U+E538 -> U+25FB
  ["ee94b9", "e297bc"], # [BLACK MEDIUM SQUARE] U+E539 -> U+25FC
  ["ee9586", :undef], # [LARGE ORANGE DIAMOND] U+E546 -> U+1F536
  ["ee9587", :undef], # [LARGE BLUE DIAMOND] U+E547 -> U+1F537
  ["ee94b6", :undef], # [SMALL ORANGE DIAMOND] U+E536 -> U+1F538
  ["ee94b7", :undef], # [SMALL BLUE DIAMOND] U+E537 -> U+1F539
  ["ee91ac", "e29d87"], # [SPARKLE] U+E46C -> U+2747
  ["ee93b0", :undef], # [WHITE FLOWER] U+E4F0 -> U+1F4AE
  ["ee93b2", :undef], # [HUNDRED POINTS SYMBOL] U+E4F2 -> U+1F4AF
  ["ee959d", "e286a9"], # [LEFTWARDS ARROW WITH HOOK] U+E55D -> U+21A9
  ["ee959c", "e286aa"], # [RIGHTWARDS ARROW WITH HOOK] U+E55C -> U+21AA
  ["eeac8d", :undef], # [CLOCKWISE DOWNWARDS AND UPWARDS OPEN CIRCLE ARROWS] U+EB0D -> U+1F503
  ["ee9491", :undef], # [SPEAKER WITH THREE SOUND WAVES] U+E511 -> U+1F50A
  ["ee9684", :undef], # [BATTERY] U+E584 -> U+1F50B
  ["ee9689", :undef], # [ELECTRIC PLUG] U+E589 -> U+1F50C
  ["ee9498", :undef], # [LEFT-POINTING MAGNIFYING GLASS] U+E518 -> U+1F50D
  ["eeac85", :undef], # [RIGHT-POINTING MAGNIFYING GLASS] U+EB05 -> U+1F50E
  ["ee949c", :undef], # [LOCK] U+E51C -> U+1F512
  ["eeac8c", :undef], # [LOCK WITH INK PEN] U+EB0C -> U+1F50F
  ["eeabbc", :undef], # [CLOSED LOCK WITH KEY] U+EAFC -> U+1F510
  ["ee9499", :undef], # [KEY] U+E519 -> U+1F511
  ["ee9492", :undef], # [BELL] U+E512 -> U+1F514
  ["eeac82", "e29891"], # [BALLOT BOX WITH CHECK] U+EB02 -> U+2611
  ["eeac84", :undef], # [RADIO BUTTON] U+EB04 -> U+1F518
  ["eeac87", :undef], # [BOOKMARK] U+EB07 -> U+1F516
  ["ee968a", :undef], # [LINK SYMBOL] U+E58A -> U+1F517
  ["eeac86", :undef], # [BACK WITH LEFTWARDS ARROW ABOVE] U+EB06 -> U+1F519
  ["ee968c", "e28083"], # [EM SPACE] U+E58C -> U+2003
  ["ee968d", "e28082"], # [EN SPACE] U+E58D -> U+2002
  ["ee968e", "e28085"], # [FOUR-PER-EM SPACE] U+E58E -> U+2005
  ["ee959e", :undef], # [WHITE HEAVY CHECK MARK] U+E55E -> U+2705
  ["eeae83", :undef], # [RAISED FIST] U+EB83 -> U+270A
  ["ee96a7", :undef], # [RAISED HAND] U+E5A7 -> U+270B
  ["ee96a6", "e29c8c"], # [VICTORY HAND] U+E5A6 -> U+270C
  ["ee93b3", :undef], # [FISTED HAND SIGN] U+E4F3 -> U+1F44A
  ["ee93b9", :undef], # [THUMBS UP SIGN] U+E4F9 -> U+1F44D
  ["ee93b6", "e2989d"], # [WHITE UP POINTING INDEX] U+E4F6 -> U+261D
  ["eeaa8d", :undef], # [WHITE UP POINTING BACKHAND INDEX] U+EA8D -> U+1F446
  ["eeaa8e", :undef], # [WHITE DOWN POINTING BACKHAND INDEX] U+EA8E -> U+1F447
  ["ee93bf", :undef], # [WHITE LEFT POINTING BACKHAND INDEX] U+E4FF -> U+1F448
  ["ee9480", :undef], # [WHITE RIGHT POINTING BACKHAND INDEX] U+E500 -> U+1F449
  ["eeab96", :undef], # [WAVING HAND SIGN] U+EAD6 -> U+1F44B
  ["eeab93", :undef], # [CLAPPING HANDS SIGN] U+EAD3 -> U+1F44F
  ["eeab94", :undef], # [OK HAND SIGN] U+EAD4 -> U+1F44C
  ["eeab95", :undef], # [THUMBS DOWN SIGN] U+EAD5 -> U+1F44E
  ["ee95b7", :undef], # [EMOJI COMPATIBILITY SYMBOL-37] U+E577 -> "[EZ]"
  ["ee96b2", :undef], # [EMOJI COMPATIBILITY SYMBOL-38] U+E5B2 -> "[ezplus]"
  ["eeaa9d", :undef], # [EMOJI COMPATIBILITY SYMBOL-39] U+EA9D -> "[EZナビ]"
  ["eeadb4", :undef], # [EMOJI COMPATIBILITY SYMBOL-40] U+EB74 -> "[EZムービー]"
  ["eeae81", :undef], # [EMOJI COMPATIBILITY SYMBOL-41] U+EB81 -> "[Cメール]"
  ["eeae89", :undef], # [EMOJI COMPATIBILITY SYMBOL-42] U+EB89 -> "[Java]"
  ["eeae8a", :undef], # [EMOJI COMPATIBILITY SYMBOL-43] U+EB8A -> "[BREW]"
  ["eeae8b", :undef], # [EMOJI COMPATIBILITY SYMBOL-44] U+EB8B -> "[EZ着うた]"
  ["eeae8c", :undef], # [EMOJI COMPATIBILITY SYMBOL-45] U+EB8C -> "[EZナビ]"
  ["eeae8d", :undef], # [EMOJI COMPATIBILITY SYMBOL-46] U+EB8D -> "[WIN]"
  ["eeae8e", :undef], # [EMOJI COMPATIBILITY SYMBOL-47] U+EB8E -> "[プレミアム]"
  ["eeabbb", :undef], # [EMOJI COMPATIBILITY SYMBOL-48] U+EAFB -> "[オープンウェブ]"
  ["ee9686", :undef], # [EMOJI COMPATIBILITY SYMBOL-49] U+E586 -> "[PDC]"
  ["ee94ac", :undef], # [EMOJI COMPATIBILITY SYMBOL-66] U+E52C -> "[Q]"
  # for undocumented codepoints
  ["eebda0", "e29880"], # [BLACK SUN WITH RAYS] U+E488 -> U+2600
  ["eebda5", "e29881"], # [CLOUD] U+E48D -> U+2601
  ["eebda4", "e29894"], # [UMBRELLA WITH RAIN DROPS] U+E48C -> U+2614
  ["eebd9d", "e29b84"], # [SNOWMAN WITHOUT SNOW] U+E485 -> U+26C4
  ["eebd9f", "e29aa1"], # [HIGH VOLTAGE SIGN] U+E487 -> U+26A1
  ["eebd81", :undef], # [CYCLONE] U+E469 -> U+1F300
  ["ef82b5", :undef], # [FOGGY] U+E598 -> U+1F301
  ["eeb2bc", :undef], # [CLOSED UMBRELLA] U+EAE8 -> U+1F302
  ["eeb385", :undef], # [NIGHT WITH STARS] U+EAF1 -> U+1F303
  ["eeb388", :undef], # [SUNRISE] U+EAF4 -> U+1F305
  ["eeb18d", :undef], # [CITYSCAPE AT DUSK] U+E5DA -> "[夕焼け]"
  ["eeb386", :undef], # [RAINBOW] U+EAF2 -> U+1F308
  ["eebda2", "e29d84"], # [SNOWFLAKE] U+E48A -> U+2744
  ["eebda6", "e29b85"], # [SUN BEHIND CLOUD] U+E48E -> U+26C5
  ["eebe98", :undef], # [BRIDGE AT NIGHT] U+E4BF -> U+1F309
  ["eeb681", :undef], # [WATER WAVE] U+EB7C -> U+1F30A
  ["eeb597", :undef], # [VOLCANO] U+EB53 -> U+1F30B
  ["eeb5a3", :undef], # [MILKY WAY] U+EB5F -> U+1F30C
  ["ef8390", :undef], # [EARTH GLOBE ASIA-AUSTRALIA] U+E5B3 -> U+1F30F
  ["ef8385", :undef], # [NEW MOON SYMBOL] U+E5A8 -> U+1F311
  ["ef8386", :undef], # [WAXING GIBBOUS MOON SYMBOL] U+E5A9 -> U+1F314
  ["ef8387", :undef], # [FIRST QUARTER MOON SYMBOL] U+E5AA -> U+1F313
  ["eebd9e", :undef], # [CRESCENT MOON] U+E486 -> U+1F319
  ["eebda1", :undef], # [FIRST QUARTER MOON WITH FACE] U+E489 -> U+1F31B
  ["eebd80", :undef], # [SHOOTING STAR] U+E468 -> U+1F320
  ["ef8297", "e28c9a"], # [WATCH] U+E57A -> U+231A
  ["ef8298", "e28c9b"], # [HOURGLASS] U+E57B -> U+231B
  ["ef82b1", :undef], # [ALARM CLOCK] U+E594 -> U+23F0
  ["eebd94", :undef], # [HOURGLASS WITH FLOWING SAND] U+E47C -> U+23F3
  ["eebda7", "e29988"], # [ARIES] U+E48F -> U+2648
  ["eebda8", "e29989"], # [TAURUS] U+E490 -> U+2649
  ["eebda9", "e2998a"], # [GEMINI] U+E491 -> U+264A
  ["eebdaa", "e2998b"], # [CANCER] U+E492 -> U+264B
  ["eebdab", "e2998c"], # [LEO] U+E493 -> U+264C
  ["eebdac", "e2998d"], # [VIRGO] U+E494 -> U+264D
  ["eebdad", "e2998e"], # [LIBRA] U+E495 -> U+264E
  ["eebdae", "e2998f"], # [SCORPIUS] U+E496 -> U+264F
  ["eebdaf", "e29990"], # [SAGITTARIUS] U+E497 -> U+2650
  ["eebdb0", "e29991"], # [CAPRICORN] U+E498 -> U+2651
  ["eebdb1", "e29992"], # [AQUARIUS] U+E499 -> U+2652
  ["eebdb2", "e29993"], # [PISCES] U+E49A -> U+2653
  ["eebdb3", :undef], # [OPHIUCHUS] U+E49B -> U+26CE
  ["eebfac", :undef], # [FOUR LEAF CLOVER] U+E513 -> U+1F340
  ["eebebd", :undef], # [TULIP] U+E4E4 -> U+1F337
  ["eeb682", :undef], # [SEEDLING] U+EB7D -> U+1F331
  ["eebea7", :undef], # [MAPLE LEAF] U+E4CE -> U+1F341
  ["eebea3", :undef], # [CHERRY BLOSSOM] U+E4CA -> U+1F338
  ["ef83aa", :undef], # [ROSE] U+E5BA -> U+1F339
  ["eeb180", :undef], # [FALLEN LEAF] U+E5CD -> U+1F342
  ["eeb1a7", :undef], # [HIBISCUS] U+EA94 -> U+1F33A
  ["eebebc", :undef], # [SUNFLOWER] U+E4E3 -> U+1F33B
  ["eebebb", :undef], # [PALM TREE] U+E4E2 -> U+1F334
  ["eeb1a9", :undef], # [CACTUS] U+EA96 -> U+1F335
  ["eeb3b7", :undef], # [EAR OF MAIZE] U+EB36 -> U+1F33D
  ["eeb3b8", :undef], # [MUSHROOM] U+EB37 -> U+1F344
  ["eeb3b9", :undef], # [CHESTNUT] U+EB38 -> U+1F330
  ["eeb58d", :undef], # [BLOSSOM] U+EB49 -> U+1F33C
  ["eeb687", :undef], # [HERB] U+EB82 -> U+1F33F
  ["eebeab", :undef], # [CHERRIES] U+E4D2 -> U+1F352
  ["eeb3b6", :undef], # [BANANA] U+EB35 -> U+1F34C
  ["eeb28d", :undef], # [RED APPLE] U+EAB9 -> U+1F34E
  ["eeb28e", :undef], # [TANGERINE] U+EABA -> U+1F34A
  ["eebead", :undef], # [STRAWBERRY] U+E4D4 -> U+1F353
  ["eebea6", :undef], # [WATERMELON] U+E4CD -> U+1F349
  ["eeb28f", :undef], # [TOMATO] U+EABB -> U+1F345
  ["eeb290", :undef], # [AUBERGINE] U+EABC -> U+1F346
  ["eeb3b3", :undef], # [MELON] U+EB32 -> U+1F348
  ["eeb3b4", :undef], # [PINEAPPLE] U+EB33 -> U+1F34D
  ["eeb3b5", :undef], # [GRAPES] U+EB34 -> U+1F347
  ["eeb3ba", :undef], # [PEACH] U+EB39 -> U+1F351
  ["eeb59e", :undef], # [GREEN APPLE] U+EB5A -> U+1F34F
  ["ef8381", :undef], # [EYES] U+E5A4 -> U+1F440
  ["ef8382", :undef], # [EAR] U+E5A5 -> U+1F442
  ["eeb2a4", :undef], # [NOSE] U+EAD0 -> U+1F443
  ["eeb2a5", :undef], # [MOUTH] U+EAD1 -> U+1F444
  ["eeb58b", :undef], # [TONGUE] U+EB47 -> U+1F445
  ["eebfa2", :undef], # [LIPSTICK] U+E509 -> U+1F484
  ["eeb1b3", :undef], # [NAIL POLISH] U+EAA0 -> U+1F485
  ["eebfa4", :undef], # [FACE MASSAGE] U+E50B -> U+1F486
  ["eeb1b4", :undef], # [HAIRCUT] U+EAA1 -> U+1F487
  ["eeb1b5", :undef], # [BARBER POLE] U+EAA2 -> U+1F488
  ["eebf95", :undef], # [BOY] U+E4FC -> U+1F466
  ["eebf93", :undef], # [GIRL] U+E4FA -> U+1F467
  ["eebf9a", :undef], # [FAMILY] U+E501 -> U+1F46A
  ["eeb190", :undef], # [POLICE OFFICER] U+E5DD -> U+1F46E
  ["eeb2af", :undef], # [WOMAN WITH BUNNY EARS] U+EADB -> U+1F46F
  ["eeb2bd", :undef], # [BRIDE WITH VEIL] U+EAE9 -> U+1F470
  ["eeb394", :undef], # [WESTERN PERSON] U+EB13 -> U+1F471
  ["eeb395", :undef], # [MAN WITH GUA PI MAO] U+EB14 -> U+1F472
  ["eeb396", :undef], # [MAN WITH TURBAN] U+EB15 -> U+1F473
  ["eeb397", :undef], # [OLDER MAN] U+EB16 -> U+1F474
  ["eeb398", :undef], # [OLDER WOMAN] U+EB17 -> U+1F475
  ["eeb399", :undef], # [BABY] U+EB18 -> U+1F476
  ["eeb39a", :undef], # [CONSTRUCTION WORKER] U+EB19 -> U+1F477
  ["eeb39b", :undef], # [PRINCESS] U+EB1A -> U+1F478
  ["eeb588", :undef], # [JAPANESE OGRE] U+EB44 -> U+1F479
  ["eeb589", :undef], # [JAPANESE GOBLIN] U+EB45 -> U+1F47A
  ["eebea4", :undef], # [GHOST] U+E4CB -> U+1F47B
  ["ef83af", :undef], # [BABY ANGEL] U+E5BF -> U+1F47C
  ["eebfa7", :undef], # [EXTRATERRESTRIAL ALIEN] U+E50E -> U+1F47D
  ["eebf85", :undef], # [ALIEN MONSTER] U+E4EC -> U+1F47E
  ["eebf88", :undef], # [IMP] U+E4EF -> U+1F47F
  ["eebf91", :undef], # [SKULL] U+E4F8 -> U+1F480
  ["eeb39d", :undef], # [DANCER] U+EB1C -> U+1F483
  ["eeb683", :undef], # [SNAIL] U+EB7E -> U+1F40C
  ["eeb3a3", :undef], # [SNAKE] U+EB22 -> U+1F40D
  ["eeb3a4", :undef], # [CHICKEN] U+EB23 -> U+1F414
  ["eeb3a5", :undef], # [BOAR] U+EB24 -> U+1F417
  ["eeb3a6", :undef], # [BACTRIAN CAMEL] U+EB25 -> U+1F42B
  ["eeb3a0", :undef], # [ELEPHANT] U+EB1F -> U+1F418
  ["eeb3a1", :undef], # [KOALA] U+EB20 -> U+1F428
  ["ef83b7", :undef], # [OCTOPUS] U+E5C7 -> U+1F419
  ["eeb380", :undef], # [SPIRAL SHELL] U+EAEC -> U+1F41A
  ["eeb39f", :undef], # [BUG] U+EB1E -> U+1F41B
  ["eebeb6", :undef], # [ANT] U+E4DD -> U+1F41C
  ["eeb59b", :undef], # [HONEYBEE] U+EB57 -> U+1F41D
  ["eeb59c", :undef], # [LADY BEETLE] U+EB58 -> U+1F41E
  ["eeb39e", :undef], # [TROPICAL FISH] U+EB1D -> U+1F420
  ["eebeac", :undef], # [BLOWFISH] U+E4D3 -> U+1F421
  ["eeb187", :undef], # [TURTLE] U+E5D4 -> U+1F422
  ["eebeb9", :undef], # [BABY CHICK] U+E4E0 -> U+1F424
  ["eeb5ba", :undef], # [FRONT-FACING BABY CHICK] U+EB76 -> U+1F425
  ["eeb18e", :undef], # [HATCHING CHICK] U+E5DB -> U+1F423
  ["eebeb5", :undef], # [PENGUIN] U+E4DC -> U+1F427
  ["eebeb8", :undef], # [POODLE] U+E4DF -> U+1F429
  ["eeb39c", :undef], # [DOLPHIN] U+EB1B -> U+1F42C
  ["ef83b2", :undef], # [MOUSE FACE] U+E5C2 -> U+1F42D
  ["ef83b0", :undef], # [TIGER FACE] U+E5C0 -> U+1F42F
  ["eebeb4", :undef], # [CAT FACE] U+E4DB -> U+1F431
  ["eebd88", :undef], # [SPOUTING WHALE] U+E470 -> U+1F433
  ["eebeb1", :undef], # [HORSE FACE] U+E4D8 -> U+1F434
  ["eebeb2", :undef], # [MONKEY FACE] U+E4D9 -> U+1F435
  ["eebeba", :undef], # [DOG FACE] U+E4E1 -> U+1F436
  ["eebeb7", :undef], # [PIG FACE] U+E4DE -> U+1F437
  ["ef83b1", :undef], # [BEAR FACE] U+E5C1 -> U+1F43B
  ["eeb3a2", :undef], # [COW FACE] U+EB21 -> U+1F42E
  ["eebeb0", :undef], # [RABBIT FACE] U+E4D7 -> U+1F430
  ["eebeb3", :undef], # [FROG FACE] U+E4DA -> U+1F438
  ["eebf87", :undef], # [PAW PRINTS] U+E4EE -> U+1F43E
  ["eeb583", :undef], # [DRAGON FACE] U+EB3F -> U+1F432
  ["eeb58a", :undef], # [PANDA FACE] U+EB46 -> U+1F43C
  ["eeb58c", :undef], # [PIG NOSE] U+EB48 -> U+1F43D
  ["eebd8a", :undef], # [ANGRY FACE] U+E472 -> U+1F600
  ["eeb5ab", :undef], # [ANGUISHED FACE] U+EB67 -> U+1F601
  ["eeb29e", :undef], # [ASTONISHED FACE] U+EACA -> U+1F602
  ["ef838b", :undef], # [DIZZY FACE] U+E5AE -> U+1F604
  ["eeb29f", :undef], # [EXASPERATED FACE] U+EACB -> U+1F605
  ["eeb29d", :undef], # [EXPRESSIONLESS FACE] U+EAC9 -> U+1F606
  ["ef83b4", :undef], # [FACE WITH HEART-SHAPED EYES] U+E5C4 -> U+1F607
  ["eeb295", :undef], # [FACE WITH LOOK OF TRIUMPH] U+EAC1 -> U+1F608
  ["eebf80", :undef], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E4E7 -> U+1F609
  ["eeb2a3", :undef], # [FACE THROWING A KISS] U+EACF -> U+1F60C
  ["eeb2a2", :undef], # [FACE KISSING] U+EACE -> U+1F60D
  ["eeb29b", :undef], # [FACE WITH MASK] U+EAC7 -> U+1F60E
  ["eeb29c", :undef], # [FLUSHED FACE] U+EAC8 -> U+1F60F
  ["eebd89", :undef], # [HAPPY FACE WITH OPEN MOUTH] U+E471 -> U+1F610
  ["eeb685", :undef], # [HAPPY FACE WITH GRIN] U+EB80 -> U+1F613
  ["eeb5a8", :undef], # [HAPPY AND CRYING FACE] U+EB64 -> U+1F614
  ["eeb2a1", :undef], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+EACD -> U+1F615
  ["eebf94", "e298ba"], # [WHITE SMILING FACE] U+E4FB -> U+263A
  ["eeb5ad", :undef], # [CRYING FACE] U+EB69 -> U+1F617
  ["eebd8b", :undef], # [LOUDLY CRYING FACE] U+E473 -> U+1F618
  ["eeb29a", :undef], # [FEARFUL FACE] U+EAC6 -> U+1F619
  ["eeb296", :undef], # [PERSEVERING FACE] U+EAC2 -> U+1F61A
  ["eeb5a1", :undef], # [POUTING FACE] U+EB5D -> U+1F61B
  ["eeb299", :undef], # [RELIEVED FACE] U+EAC5 -> U+1F61C
  ["eeb297", :undef], # [CONFOUNDED FACE] U+EAC3 -> U+1F61D
  ["eeb294", :undef], # [PENSIVE FACE] U+EAC0 -> U+1F61E
  ["ef83b5", :undef], # [FACE SCREAMING IN FEAR] U+E5C5 -> U+1F61F
  ["eeb298", :undef], # [SLEEPY FACE] U+EAC4 -> U+1F620
  ["eeb293", :undef], # [SMIRKING FACE] U+EABF -> U+1F621
  ["ef83b6", :undef], # [FACE WITH COLD SWEAT] U+E5C6 -> U+1F622
  ["eebd8c", :undef], # [TIRED FACE] U+E474 -> U+1F624
  ["ef83b3", :undef], # [WINKING FACE] U+E5C3 -> U+1F625
  ["eeb5a5", :undef], # [CAT FACE WITH OPEN MOUTH] U+EB61 -> U+1F62B
  ["eeb684", :undef], # [HAPPY CAT FACE WITH GRIN] U+EB7F -> U+1F62C
  ["eeb5a7", :undef], # [HAPPY AND CRYING CAT FACE] U+EB63 -> U+1F62D
  ["eeb5a4", :undef], # [CAT FACE KISSING] U+EB60 -> U+1F62E
  ["eeb5a9", :undef], # [CAT FACE WITH HEART-SHAPED EYES] U+EB65 -> U+1F62F
  ["eeb5ac", :undef], # [CRYING CAT FACE] U+EB68 -> U+1F630
  ["eeb5a2", :undef], # [POUTING CAT FACE] U+EB5E -> U+1F631
  ["eeb5ae", :undef], # [CAT FACE WITH TIGHTLY-CLOSED LIPS] U+EB6A -> U+1F632
  ["eeb5aa", :undef], # [ANGUISHED CAT FACE] U+EB66 -> U+1F633
  ["eeb2ab", :undef], # [FACE WITH NO GOOD GESTURE] U+EAD7 -> U+1F634
  ["eeb2ac", :undef], # [FACE WITH OK GESTURE] U+EAD8 -> U+1F635
  ["eeb2ad", :undef], # [PERSON BOWING DEEPLY] U+EAD9 -> U+1F636
  ["eeb594", :undef], # [SEE-NO-EVIL MONKEY] U+EB50 -> U+1F637
  ["eeb595", :undef], # [SPEAK-NO-EVIL MONKEY] U+EB51 -> U+1F639
  ["eeb596", :undef], # [HEAR-NO-EVIL MONKEY] U+EB52 -> U+1F638
  ["eeb68a", :undef], # [PERSON RAISING ONE HAND] U+EB85 -> U+1F63A
  ["eeb68b", :undef], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+EB86 -> U+1F63B
  ["eeb68c", :undef], # [PERSON FROWNING] U+EB87 -> U+1F63C
  ["eeb68d", :undef], # [PERSON WITH POUTING FACE] U+EB88 -> U+1F63D
  ["eeb2a6", :undef], # [PERSON WITH FOLDED HANDS] U+EAD2 -> U+1F63E
  ["eebe84", :undef], # [HOUSE BUILDING] U+E4AB -> U+1F3E0
  ["ef83a0", :undef], # [HOUSE WITH GARDEN] U+EB09 -> U+1F3E1
  ["eebe86", :undef], # [OFFICE BUILDING] U+E4AD -> U+1F3E2
  ["eeb191", :undef], # [JAPANESE POST OFFICE] U+E5DE -> U+1F3E3
  ["eeb192", :undef], # [HOSPITAL] U+E5DF -> U+1F3E5
  ["eebe83", :undef], # [BANK] U+E4AA -> U+1F3E6
  ["eebdbb", :undef], # [AUTOMATED TELLER MACHINE] U+E4A3 -> U+1F3E7
  ["eeb194", :undef], # [HOTEL] U+EA81 -> U+1F3E8
  ["eeb387", :undef], # [LOVE HOTEL] U+EAF3 -> U+1F3E9
  ["eebdbc", :undef], # [CONVENIENCE STORE] U+E4A4 -> U+1F3EA
  ["eeb193", :undef], # [SCHOOL] U+EA80 -> U+1F3EB
  ["ef83ab", "e29baa"], # [CHURCH] U+E5BB -> U+26EA
  ["eeb182", "e29bb2"], # [FOUNTAIN] U+E5CF -> U+26F2
  ["eeb38a", :undef], # [DEPARTMENT STORE] U+EAF6 -> U+1F3EC
  ["eeb38b", :undef], # [JAPANESE CASTLE] U+EAF7 -> U+1F3EF
  ["eeb38c", :undef], # [EUROPEAN CASTLE] U+EAF8 -> U+1F3F0
  ["eeb38d", :undef], # [FACTORY] U+EAF9 -> U+1F3ED
  ["eebe82", "e29a93"], # [ANCHOR] U+E4A9 -> U+2693
  ["eebe96", :undef], # [IZAKAYA LANTERN] U+E4BD -> U+1F3EE
  ["ef83ad", :undef], # [MOUNT FUJI] U+E5BD -> U+1F5FB
  ["eebe99", :undef], # [TOKYO TOWER] U+E4C0 -> U+1F5FC
  ["ef828f", :undef], # [SILHOUETTE OF JAPAN] U+E572 -> U+1F5FE
  ["eeb5b0", :undef], # [MOYAI] U+EB6C -> U+1F5FF
  ["ef83a7", :undef], # [MANS SHOE] U+E5B7 -> U+1F45E
  ["eeb3ac", :undef], # [ATHLETIC SHOE] U+EB2B -> U+1F45F
  ["eebfb3", :undef], # [HIGH-HEELED SHOE] U+E51A -> U+1F460
  ["eeb1b2", :undef], # [WOMANS BOOTS] U+EA9F -> U+1F462
  ["eeb3ab", :undef], # [FOOTPRINTS] U+EB2A -> U+1F463
  ["eebf97", :undef], # [EYEGLASSES] U+E4FE -> U+1F453
  ["ef83a6", :undef], # [T-SHIRT] U+E5B6 -> U+1F455
  ["eeb5bb", :undef], # [JEANS] U+EB77 -> U+1F456
  ["ef83b9", :undef], # [CROWN] U+E5C9 -> U+1F451
  ["eeb1a6", :undef], # [NECKTIE] U+EA93 -> U+1F454
  ["eeb1b1", :undef], # [WOMANS HAT] U+EA9E -> U+1F452
  ["eeb5af", :undef], # [DRESS] U+EB6B -> U+1F457
  ["eeb1b6", :undef], # [KIMONO] U+EAA3 -> U+1F458
  ["eeb1b7", :undef], # [BIKINI] U+EAA4 -> U+1F459
  ["eebfa6", :undef], # [WOMANS CLOTHES] U+E50D -> U+1F45A
  ["eebf9d", :undef], # [PURSE] U+E504 -> U+1F45B
  ["eebdb4", :undef], # [HANDBAG] U+E49C -> U+1F45C
  ["eebea0", :undef], # [MONEY BAG] U+E4C7 -> U+1F4B0
  ["eeb18f", :undef], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+E5DC -> U+1F4B9
  ["ef8296", :undef], # [HEAVY DOLLAR SIGN] U+E579 -> U+1F4B2
  ["ef8299", :undef], # [CREDIT CARD] U+E57C -> U+1F4B3
  ["ef829a", :undef], # [BANKNOTE WITH YEN SIGN] U+E57D -> U+1F4B4
  ["ef82a2", :undef], # [BANKNOTE WITH DOLLAR SIGN] U+E585 -> U+1F4B5
  ["eeb59f", :undef], # [MONEY WITH WINGS] U+EB5B -> U+1F4B8
  ["eeb392", "f09f87a8f09f87b3"], # [REGIONAL INDICATOR SYMBOL LETTERS CN] U+EB11 -> U+1F1E8 U+1F1F3
  ["eeb38f", "f09f87a9f09f87aa"], # [REGIONAL INDICATOR SYMBOL LETTERS DE] U+EB0E -> U+1F1E9 U+1F1EA
  ["eeb188", "f09f87aaf09f87b8"], # [REGIONAL INDICATOR SYMBOL LETTERS ES] U+E5D5 -> U+1F1EA U+1F1F8
  ["eeb38e", "f09f87abf09f87b7"], # [REGIONAL INDICATOR SYMBOL LETTERS FR] U+EAFA -> U+1F1EB U+1F1F7
  ["eeb391", "f09f87baf09f87b0"], # [REGIONAL INDICATOR SYMBOL LETTERS GB] U+EB10 -> U+1F1FA U+1F1F0
  ["eeb390", "f09f87aef09f87b9"], # [REGIONAL INDICATOR SYMBOL LETTERS IT] U+EB0F -> U+1F1EE U+1F1F9
  ["eebea5", "f09f87aff09f87b5"], # [REGIONAL INDICATOR SYMBOL LETTERS JP] U+E4CC -> U+1F1EF U+1F1F5
  ["eeb393", "f09f87b0f09f87b7"], # [REGIONAL INDICATOR SYMBOL LETTERS KR] U+EB12 -> U+1F1F0 U+1F1F7
  ["eeb189", "f09f87b7f09f87ba"], # [REGIONAL INDICATOR SYMBOL LETTERS RU] U+E5D6 -> U+1F1F7 U+1F1FA
  ["ef8290", "f09f87baf09f87b8"], # [REGIONAL INDICATOR SYMBOL LETTERS US] U+E573 -> U+1F1FA U+1F1F8
  ["eebd93", :undef], # [FIRE] U+E47B -> U+1F525
  ["ef82a0", :undef], # [ELECTRIC TORCH] U+E583 -> U+1F526
  ["ef82a4", :undef], # [WRENCH] U+E587 -> U+1F527
  ["ef83bb", :undef], # [HAMMER] U+E5CB -> U+1F528
  ["ef829e", :undef], # [NUT AND BOLT] U+E581 -> U+1F529
  ["ef829c", :undef], # [HOCHO] U+E57F -> U+1F52A
  ["eebfa3", :undef], # [PISTOL] U+E50A -> U+1F52B
  ["eeb1a2", :undef], # [CRYSTAL BALL] U+EA8F -> U+1F52E
  ["eebd98", :undef], # [JAPANESE SYMBOL FOR BEGINNER] U+E480 -> U+1F530
  ["eebfa9", :undef], # [SYRINGE] U+E510 -> U+1F489
  ["eeb1ad", :undef], # [PILL] U+EA9A -> U+1F48A
  ["eeb3a7", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+EB26 -> U+1F170
  ["eeb3a8", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+EB27 -> U+1F171
  ["eeb3aa", :undef], # [NEGATIVE SQUARED AB] U+EB29 -> U+1F18E
  ["eeb3a9", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+EB28 -> U+1F17E
  ["ef82bc", :undef], # [RIBBON] U+E59F -> U+1F380
  ["eebea8", :undef], # [WRAPPED PRESENT] U+E4CF -> U+1F381
  ["ef82bd", :undef], # [BIRTHDAY CAKE] U+E5A0 -> U+1F382
  ["eebea2", :undef], # [CHRISTMAS TREE] U+E4C9 -> U+1F384
  ["eeb384", :undef], # [FATHER CHRISTMAS] U+EAF0 -> U+1F385
  ["eeb18c", :undef], # [CROSSED FLAGS] U+E5D9 -> U+1F38C
  ["ef83bc", :undef], # [FIREWORKS] U+E5CC -> U+1F386
  ["eeb1ae", :undef], # [BALLOON] U+EA9B -> U+1F388
  ["eeb1af", :undef], # [PARTY POPPER] U+EA9C -> U+1F389
  ["eeb2b7", :undef], # [PINE DECORATION] U+EAE3 -> U+1F38D
  ["eeb2b8", :undef], # [JAPANESE DOLLS] U+EAE4 -> U+1F38E
  ["eeb2b9", :undef], # [GRADUATION CAP] U+EAE5 -> U+1F393
  ["eeb2ba", :undef], # [SCHOOL SATCHEL] U+EAE6 -> U+1F392
  ["eeb2bb", :undef], # [CARP STREAMER] U+EAE7 -> U+1F38F
  ["eeb2bf", :undef], # [FIREWORK SPARKLER] U+EAEB -> U+1F387
  ["eeb381", :undef], # [WIND CHIME] U+EAED -> U+1F390
  ["eeb382", :undef], # [JACK-O-LANTERN] U+EAEE -> U+1F383
  ["eebd87", :undef], # [CONFETTI BALL] U+E46F -> U+1F38A
  ["eeb581", :undef], # [TANABATA TREE] U+EB3D -> U+1F38B
  ["eeb383", :undef], # [MOON VIEWING CEREMONY] U+EAEF -> U+1F391
  ["ef82b8", :undef], # [PAGER] U+E59B -> U+1F4DF
  ["ef82b3", "e2988e"], # [BLACK TELEPHONE] U+E596 -> U+260E
  ["eebfb7", :undef], # [TELEPHONE RECEIVER] U+E51E -> U+1F4DE
  ["ef82a5", :undef], # [MOBILE PHONE] U+E588 -> U+1F4F1
  ["ef839f", :undef], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+EB08 -> U+1F4F2
  ["eeb1a5", :undef], # [MEMO] U+EA92 -> U+1F4DD
  ["eebfb9", :undef], # [FAX MACHINE] U+E520 -> U+1F4E0
  ["eebfba", "e29c89"], # [ENVELOPE] U+E521 -> U+2709
  ["ef82ae", :undef], # [INCOMING ENVELOPE] U+E591 -> U+1F4E8
  ["eeb5a6", :undef], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+EB62 -> U+1F4E9
  ["eebfb4", :undef], # [CLOSED MAILBOX WITH LOWERED FLAG] U+E51B -> U+1F4EA
  ["ef83a1", :undef], # [CLOSED MAILBOX WITH RAISED FLAG] U+EB0A -> U+1F4EB
  ["ef82a8", :undef], # [NEWSPAPER] U+E58B -> U+1F4F0
  ["eebe81", :undef], # [SATELLITE ANTENNA] U+E4A8 -> U+1F4E1
  ["ef82af", :undef], # [OUTBOX TRAY] U+E592 -> U+1F4E4
  ["ef82b0", :undef], # [INBOX TRAY] U+E593 -> U+1F4E5
  ["eebfb8", :undef], # [PACKAGE] U+E51F -> U+1F4E6
  ["eeb5b5", :undef], # [E-MAIL SYMBOL] U+EB71 -> U+1F4E7
  ["ef8394", :undef], # [INPUT SYMBOL FOR LATIN CAPITAL LETTERS] U+EAFD -> U+1F520
  ["ef8395", :undef], # [INPUT SYMBOL FOR LATIN SMALL LETTERS] U+EAFE -> U+1F521
  ["ef8396", :undef], # [INPUT SYMBOL FOR NUMBERS] U+EAFF -> U+1F522
  ["ef8397", :undef], # [INPUT SYMBOL FOR SYMBOLS] U+EB00 -> U+1F523
  ["eeb599", :undef], # [INPUT SYMBOL FOR LATIN LETTERS] U+EB55 -> U+1F524
  ["ef839a", "e29c92"], # [BLACK NIB] U+EB03 -> U+2712
  ["ef83a8", :undef], # [PERSONAL COMPUTER] U+E5B8 -> U+1F4BB
  ["eebdb9", "e29c8f"], # [PENCIL] U+E4A1 -> U+270F
  ["eebdb8", :undef], # [PAPERCLIP] U+E4A0 -> U+1F4CE
  ["eeb181", :undef], # [BRIEFCASE] U+E5CE -> U+1F4BC
  ["ef829f", :undef], # [MINIDISC] U+E582 -> U+1F4BD
  ["ef81be", :undef], # [FLOPPY DISK] U+E562 -> U+1F4BE
  ["eebfa5", :undef], # [OPTICAL DISC] U+E50C -> U+1F4BF
  ["eebfaf", "e29c82"], # [BLACK SCISSORS] U+E516 -> U+2702
  ["ef81bc", :undef], # [ROUND PUSHPIN] U+E560 -> U+1F4CD
  ["ef81bd", :undef], # [PAGE WITH CURL] U+E561 -> U+1F4C3
  ["ef8286", :undef], # [PAGE FACING UP] U+E569 -> U+1F4C4
  ["ef8280", :undef], # [CALENDAR] U+E563 -> U+1F4C5
  ["ef82ac", :undef], # [FILE FOLDER] U+E58F -> U+1F4C1
  ["ef82ad", :undef], # [OPEN FILE FOLDER] U+E590 -> U+1F4C2
  ["ef8288", :undef], # [NOTEBOOK] U+E56B -> U+1F4D3
  ["eebdb7", :undef], # [OPEN BOOK] U+E49F -> U+1F4D6
  ["eebdb5", :undef], # [NOTEBOOK WITH DECORATIVE COVER] U+E49D -> U+1F4D4
  ["ef8285", :undef], # [CLOSED BOOK] U+E568 -> U+1F4D5
  ["ef8282", :undef], # [GREEN BOOK] U+E565 -> U+1F4D7
  ["ef8283", :undef], # [BLUE BOOK] U+E566 -> U+1F4D8
  ["ef8284", :undef], # [ORANGE BOOK] U+E567 -> U+1F4D9
  ["ef828c", :undef], # [BOOKS] U+E56F -> U+1F4DA
  ["eebfb6", :undef], # [NAME BADGE] U+E51D -> U+1F4DB
  ["ef81bb", :undef], # [SCROLL] U+E55F -> U+1F4DC
  ["ef8281", :undef], # [CLIPBOARD] U+E564 -> U+1F4CB
  ["ef8287", :undef], # [TEAR-OFF CALENDAR] U+E56A -> U+1F4C6
  ["ef8291", :undef], # [BAR CHART] U+E574 -> U+1F4CA
  ["ef8292", :undef], # [CHART WITH UPWARDS TREND] U+E575 -> U+1F4C8
  ["ef8293", :undef], # [CHART WITH DOWNWARDS TREND] U+E576 -> U+1F4C9
  ["ef8289", :undef], # [CARD INDEX] U+E56C -> U+1F4C7
  ["ef828a", :undef], # [PUSHPIN] U+E56D -> U+1F4CC
  ["ef828b", :undef], # [LEDGER] U+E56E -> U+1F4D2
  ["ef828d", :undef], # [STRAIGHT RULER] U+E570 -> U+1F4CF
  ["eebdba", :undef], # [TRIANGULAR RULER] U+E4A2 -> U+1F4D0
  ["ef83a2", :undef], # [BOOKMARK TABS] U+EB0B -> U+1F4D1
  ["eebe93", "e29abe"], # [BASEBALL] U+E4BA -> U+26BE
  ["ef82b6", "e29bb3"], # [FLAG IN HOLE] U+E599 -> U+26F3
  ["eebe90", :undef], # [TENNIS RACQUET AND BALL] U+E4B7 -> U+1F3BE
  ["eebe8f", "e29abd"], # [SOCCER BALL] U+E4B6 -> U+26BD
  ["eeb280", :undef], # [SKI AND SKI BOOT] U+EAAC -> U+1F3BF
  ["ef82b7", :undef], # [BASKETBALL AND HOOP] U+E59A -> U+1F3C0
  ["eebe92", :undef], # [CHEQUERED FLAG] U+E4B9 -> U+1F3C1
  ["eebe91", :undef], # [SNOWBOARDER] U+E4B8 -> U+1F3C2
  ["eebd83", :undef], # [RUNNER] U+E46B -> U+1F3C3
  ["eeb585", :undef], # [SURFER] U+EB41 -> U+1F3C4
  ["eeb186", :undef], # [TROPHY] U+E5D3 -> U+1F3C6
  ["eebe94", :undef], # [AMERICAN FOOTBALL] U+E4BB -> U+1F3C8
  ["eeb2b2", :undef], # [SWIMMER] U+EADE -> U+1F3CA
  ["eebe8e", :undef], # [TRAIN] U+E4B5 -> U+1F686
  ["ef83ac", :undef], # [METRO] U+E5BC -> U+1F687
  ["eebe89", :undef], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+E4B0 -> U+1F685
  ["eebe8a", :undef], # [AUTOMOBILE] U+E4B1 -> U+1F697
  ["eebe88", :undef], # [ONCOMING BUS] U+E4AF -> U+1F68D
  ["eebe80", :undef], # [BUS STOP] U+E4A7 -> U+1F68F
  ["eeb195", :undef], # [SHIP] U+EA82 -> U+1F6A2
  ["eebe8c", "e29c88"], # [AIRPLANE] U+E4B3 -> U+2708
  ["eebe8d", "e29bb5"], # [SAILBOAT] U+E4B4 -> U+26F5
  ["eeb5b1", :undef], # [STATION] U+EB6D -> U+1F689
  ["ef83b8", :undef], # [ROCKET] U+E5C8 -> U+1F680
  ["eebe8b", :undef], # [DELIVERY TRUCK] U+E4B2 -> U+1F69A
  ["eeb2b3", :undef], # [FIRE ENGINE] U+EADF -> U+1F692
  ["eeb2b4", :undef], # [AMBULANCE] U+EAE0 -> U+1F691
  ["eeb2b5", :undef], # [POLICE CAR] U+EAE1 -> U+1F693
  ["ef828e", "e29bbd"], # [FUEL PUMP] U+E571 -> U+26FD
  ["eebdbe", "f09f85bf"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E4A6 -> U+1F17F
  ["eebd82", :undef], # [HORIZONTAL TRAFFIC LIGHT] U+E46A -> U+1F6A5
  ["eeb18a", :undef], # [CONSTRUCTION SIGN] U+E5D7 -> U+1F6A7
  ["eeb5b7", :undef], # [POLICE CARS REVOLVING LIGHT] U+EB73 -> U+1F6A8
  ["eebe95", "e299a8"], # [HOT SPRINGS] U+E4BC -> U+2668
  ["eeb183", "e29bba"], # [TENT] U+E5D0 -> U+26FA
  ["eebd85", :undef], # [FERRIS WHEEL] U+E46D -> U+1F3A1
  ["eeb2b6", :undef], # [ROLLER COASTER] U+EAE2 -> U+1F3A2
  ["eeb586", :undef], # [FISHING POLE AND FISH] U+EB42 -> U+1F3A3
  ["eebf9c", :undef], # [MICROPHONE] U+E503 -> U+1F3A4
  ["eebfb0", :undef], # [MOVIE CAMERA] U+E517 -> U+1F3A5
  ["eebfa1", :undef], # [HEADPHONE] U+E508 -> U+1F3A7
  ["ef82b9", :undef], # [ARTIST PALETTE] U+E59C -> U+1F3A8
  ["eeb389", :undef], # [TOP HAT] U+EAF5 -> U+1F3A9
  ["ef82bb", :undef], # [CIRCUS TENT] U+E59E -> U+1F3AA
  ["eebdb6", :undef], # [TICKET] U+E49E -> U+1F3AB
  ["eebe97", :undef], # [CLAPPER BOARD] U+E4BE -> U+1F3AC
  ["ef82ba", :undef], # [PERFORMING ARTS] U+E59D -> U+1F3AD
  ["eebe9f", :undef], # [VIDEO GAME] U+E4C6 -> U+1F3AE
  ["eeb184", "f09f8084"], # [MAHJONG TILE RED DRAGON] U+E5D1 -> U+1F004
  ["eebe9e", :undef], # [DIRECT HIT] U+E4C5 -> U+1F3AF
  ["eebd86", :undef], # [SLOT MACHINE] U+E46E -> U+1F3B0
  ["eeb2b1", :undef], # [BILLIARDS] U+EADD -> U+1F3B1
  ["eebea1", :undef], # [GAME DIE] U+E4C8 -> U+1F3B2
  ["eeb587", :undef], # [BOWLING] U+EB43 -> U+1F3B3
  ["eeb5b2", :undef], # [FLOWER PLAYING CARDS] U+EB6E -> U+1F3B4
  ["eeb5b3", :undef], # [PLAYING CARD BLACK JOKER] U+EB6F -> U+1F0CF
  ["ef83ae", :undef], # [MUSICAL NOTE] U+E5BE -> U+1F3B5
  ["eebf9e", :undef], # [MULTIPLE MUSICAL NOTES] U+E505 -> U+1F3B6
  ["eebf9f", :undef], # [GUITAR] U+E506 -> U+1F3B8
  ["eeb584", :undef], # [MUSICAL KEYBOARD] U+EB40 -> U+1F3B9
  ["eeb2b0", :undef], # [TRUMPET] U+EADC -> U+1F3BA
  ["eebfa0", :undef], # [VIOLIN] U+E507 -> U+1F3BB
  ["eeb2a0", :undef], # [MUSICAL SCORE] U+EACC -> U+1F3BC
  ["eebfae", :undef], # [CAMERA] U+E515 -> U+1F4F7
  ["ef829b", :undef], # [VIDEO CAMERA] U+E57E -> U+1F4F9
  ["eebf9b", :undef], # [TELEVISION] U+E502 -> U+1F4FA
  ["ef83a9", :undef], # [RADIO] U+E5B9 -> U+1F4FB
  ["ef829d", :undef], # [VIDEOCASSETTE] U+E580 -> U+1F4FC
  ["eebf84", :undef], # [KISS MARK] U+E4EB -> U+1F48B
  ["eeb5bc", :undef], # [LOVE LETTER] U+EB78 -> U+1F48C
  ["eebfad", :undef], # [RING] U+E514 -> U+1F48D
  ["ef83ba", :undef], # [KISS] U+E5CA -> U+1F48F
  ["eeb1a8", :undef], # [BOUQUET] U+EA95 -> U+1F490
  ["eeb2ae", :undef], # [COUPLE WITH HEART] U+EADA -> U+1F491
  ["eeb196", :undef], # [NO ONE UNDER EIGHTEEN SYMBOL] U+EA83 -> U+1F51E
  ["ef81b4", "c2a9"], # [COPYRIGHT SIGN] U+E558 -> U+A9
  ["ef81b5", "c2ae"], # [REGISTERED SIGN] U+E559 -> U+AE
  ["ef81aa", "e284a2"], # [TRADE MARK SIGN] U+E54E -> U+2122
  ["ef818f", "e284b9"], # [INFORMATION SOURCE] U+E533 -> U+2139
  ["eeb689", "23e283a3"], # [HASH KEY] U+EB84 -> U+23 U+20E3
  ["eebfbb", "31e283a3"], # [KEYCAP 1] U+E522 -> U+31 U+20E3
  ["eebfbc", "32e283a3"], # [KEYCAP 2] U+E523 -> U+32 U+20E3
  ["ef8180", "33e283a3"], # [KEYCAP 3] U+E524 -> U+33 U+20E3
  ["ef8181", "34e283a3"], # [KEYCAP 4] U+E525 -> U+34 U+20E3
  ["ef8182", "35e283a3"], # [KEYCAP 5] U+E526 -> U+35 U+20E3
  ["ef8183", "36e283a3"], # [KEYCAP 6] U+E527 -> U+36 U+20E3
  ["ef8184", "37e283a3"], # [KEYCAP 7] U+E528 -> U+37 U+20E3
  ["ef8185", "38e283a3"], # [KEYCAP 8] U+E529 -> U+38 U+20E3
  ["ef8186", "39e283a3"], # [KEYCAP 9] U+E52A -> U+39 U+20E3
  ["ef8389", "30e283a3"], # [KEYCAP 0] U+E5AC -> U+30 U+20E3
  ["ef8187", :undef], # [KEYCAP TEN] U+E52B -> U+1F51F
  ["eeb197", :undef], # [ANTENNA WITH BARS] U+EA84 -> U+1F4F6
  ["eeb1a3", :undef], # [VIBRATION MODE] U+EA90 -> U+1F4F3
  ["eeb1a4", :undef], # [MOBILE PHONE OFF] U+EA91 -> U+1F4F4
  ["eebeaf", :undef], # [HAMBURGER] U+E4D6 -> U+1F354
  ["eebeae", :undef], # [RICE BALL] U+E4D5 -> U+1F359
  ["eebea9", :undef], # [SHORTCAKE] U+E4D0 -> U+1F370
  ["ef8391", :undef], # [STEAMING BOWL] U+E5B4 -> U+1F35C
  ["eeb283", :undef], # [BREAD] U+EAAF -> U+1F35E
  ["eebeaa", :undef], # [COOKING] U+E4D1 -> U+1F373
  ["eeb284", :undef], # [SOFT ICE CREAM] U+EAB0 -> U+1F366
  ["eeb285", :undef], # [FRENCH FRIES] U+EAB1 -> U+1F35F
  ["eeb286", :undef], # [DANGO] U+EAB2 -> U+1F361
  ["eeb287", :undef], # [RICE CRACKER] U+EAB3 -> U+1F358
  ["eeb288", :undef], # [COOKED RICE] U+EAB4 -> U+1F35A
  ["eeb289", :undef], # [SPAGHETTI] U+EAB5 -> U+1F35D
  ["eeb28a", :undef], # [CURRY AND RICE] U+EAB6 -> U+1F35B
  ["eeb28b", :undef], # [ODEN] U+EAB7 -> U+1F362
  ["eeb28c", :undef], # [SUSHI] U+EAB8 -> U+1F363
  ["eeb291", :undef], # [BENTO BOX] U+EABD -> U+1F371
  ["eeb292", :undef], # [POT OF FOOD] U+EABE -> U+1F372
  ["eeb2be", :undef], # [SHAVED ICE] U+EAEA -> U+1F367
  ["eebe9d", :undef], # [MEAT ON BONE] U+E4C4 -> U+1F356
  ["eebf86", :undef], # [FISH CAKE WITH SWIRL DESIGN] U+E4ED -> U+1F365
  ["eeb3bb", :undef], # [ROASTED SWEET POTATO] U+EB3A -> U+1F360
  ["eeb3bc", :undef], # [SLICE OF PIZZA] U+EB3B -> U+1F355
  ["eeb580", :undef], # [POULTRY LEG] U+EB3C -> U+1F357
  ["eeb58e", :undef], # [ICE CREAM] U+EB4A -> U+1F368
  ["eeb58f", :undef], # [DOUGHNUT] U+EB4B -> U+1F369
  ["eeb590", :undef], # [COOKIE] U+EB4C -> U+1F36A
  ["eeb591", :undef], # [CHOCOLATE BAR] U+EB4D -> U+1F36B
  ["eeb592", :undef], # [CANDY] U+EB4E -> U+1F36C
  ["eeb593", :undef], # [LOLLIPOP] U+EB4F -> U+1F36D
  ["eeb59a", :undef], # [CUSTARD] U+EB56 -> U+1F36E
  ["eeb59d", :undef], # [HONEY POT] U+EB59 -> U+1F36F
  ["eeb5b4", :undef], # [FRIED SHRIMP] U+EB70 -> U+1F364
  ["eebe85", :undef], # [FORK AND KNIFE] U+E4AC -> U+1F374
  ["ef82b4", "e29895"], # [HOT BEVERAGE] U+E597 -> U+2615
  ["eebe9b", :undef], # [COCKTAIL GLASS] U+E4C2 -> U+1F378
  ["eebe9c", :undef], # [BEER MUG] U+E4C3 -> U+1F37A
  ["eeb282", :undef], # [TEACUP WITHOUT HANDLE] U+EAAE -> U+1F375
  ["eeb1aa", :undef], # [SAKE BOTTLE AND CUP] U+EA97 -> U+1F376
  ["eebe9a", :undef], # [WINE GLASS] U+E4C1 -> U+1F377
  ["eeb1ab", :undef], # [CLINKING BEER MUGS] U+EA98 -> U+1F37B
  ["eeb582", :undef], # [TROPICAL DRINK] U+EB3E -> U+1F379
  ["ef81b1", "e28697"], # [NORTH EAST ARROW] U+E555 -> U+2197
  ["ef81a9", "e28698"], # [SOUTH EAST ARROW] U+E54D -> U+2198
  ["ef81a8", "e28696"], # [NORTH WEST ARROW] U+E54C -> U+2196
  ["ef81b2", "e28699"], # [SOUTH WEST ARROW] U+E556 -> U+2199
  ["eeb3ae", "e2a4b4"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+EB2D -> U+2934
  ["eeb3af", "e2a4b5"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+EB2E -> U+2935
  ["eeb5be", "e28694"], # [LEFT RIGHT ARROW] U+EB7A -> U+2194
  ["eeb680", "e28695"], # [UP DOWN ARROW] U+EB7B -> U+2195
  ["ef819b", "e2ac86"], # [UPWARDS BLACK ARROW] U+E53F -> U+2B06
  ["ef819c", "e2ac87"], # [DOWNWARDS BLACK ARROW] U+E540 -> U+2B07
  ["ef81ae", "e29ea1"], # [BLACK RIGHTWARDS ARROW] U+E552 -> U+27A1
  ["ef81af", "e2ac85"], # [LEFTWARDS BLACK ARROW] U+E553 -> U+2B05
  ["ef818a", "e296b6"], # [BLACK RIGHT-POINTING TRIANGLE] U+E52E -> U+25B6
  ["ef8189", "e29780"], # [BLACK LEFT-POINTING TRIANGLE] U+E52D -> U+25C0
  ["ef818c", :undef], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+E530 -> U+23E9
  ["ef818b", :undef], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+E52F -> U+23EA
  ["ef81a1", :undef], # [BLACK UP-POINTING DOUBLE TRIANGLE] U+E545 -> U+23EB
  ["ef81a0", :undef], # [BLACK DOWN-POINTING DOUBLE TRIANGLE] U+E544 -> U+23EC
  ["ef81b6", :undef], # [UP-POINTING RED TRIANGLE] U+E55A -> U+1F53A
  ["ef81b7", :undef], # [DOWN-POINTING RED TRIANGLE] U+E55B -> U+1F53B
  ["ef819f", :undef], # [UP-POINTING SMALL RED TRIANGLE] U+E543 -> U+1F53C
  ["ef819e", :undef], # [DOWN-POINTING SMALL RED TRIANGLE] U+E542 -> U+1F53D
  ["eeb281", "e2ad95"], # [HEAVY LARGE CIRCLE] U+EAAD -> U+2B55
  ["ef81ac", :undef], # [CROSS MARK] U+E550 -> U+274C
  ["ef81ad", :undef], # [NEGATIVE SQUARED CROSS MARK] U+E551 -> U+274E
  ["eebd9a", "e29da2"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E482 -> U+2762
  ["eeb3b0", "e28189"], # [EXCLAMATION QUESTION MARK] U+EB2F -> U+2049
  ["eeb3b1", "e280bc"], # [DOUBLE EXCLAMATION MARK] U+EB30 -> U+203C
  ["eebd9b", :undef], # [BLACK QUESTION MARK ORNAMENT] U+E483 -> U+2753
  ["eeb3b2", :undef], # [CURLY LOOP] U+EB31 -> U+27B0
  ["ef82b2", "e29da4"], # [HEAVY BLACK HEART] U+E595 -> U+2764
  ["eeb5b9", :undef], # [BEATING HEART] U+EB75 -> U+1F493
  ["eebd8f", :undef], # [BROKEN HEART] U+E477 -> U+1F494
  ["eebd90", :undef], # [TWO HEARTS] U+E478 -> U+1F495
  ["eeb1b9", :undef], # [SPARKLING HEART] U+EAA6 -> U+1F496
  ["eebf83", :undef], # [HEART WITH ARROW] U+E4EA -> U+1F498
  ["eeb1ba", :undef], # [BLUE HEART] U+EAA7 -> U+1F499
  ["eeb1bb", :undef], # [GREEN HEART] U+EAA8 -> U+1F49A
  ["eeb1bc", :undef], # [YELLOW HEART] U+EAA9 -> U+1F49B
  ["eeb1bd", :undef], # [PURPLE HEART] U+EAAA -> U+1F49C
  ["eeb598", :undef], # [HEART WITH RIBBON] U+EB54 -> U+1F49D
  ["ef838c", :undef], # [REVOLVING HEARTS] U+E5AF -> U+1F49E
  ["eeb1b8", "e299a5"], # [BLACK HEART SUIT] U+EAA5 -> U+2665
  ["ef82be", "e299a0"], # [BLACK SPADE SUIT] U+E5A1 -> U+2660
  ["ef82bf", "e299a6"], # [BLACK DIAMOND SUIT] U+E5A2 -> U+2666
  ["ef8380", "e299a3"], # [BLACK CLUB SUIT] U+E5A3 -> U+2663
  ["eebd95", :undef], # [SMOKING SYMBOL] U+E47D -> U+1F6AC
  ["eebd96", :undef], # [NO SMOKING SYMBOL] U+E47E -> U+1F6AD
  ["eebd97", "e299bf"], # [WHEELCHAIR SYMBOL] U+E47F -> U+267F
  ["eeb3ad", :undef], # [TRIANGULAR FLAG ON POST] U+EB2C -> U+1F6A9
  ["eebd99", "e29aa0"], # [WARNING SIGN] U+E481 -> U+26A0
  ["eebd9c", "e29b94"], # [NO ENTRY] U+E484 -> U+26D4
  ["eeb5bd", "e299bb"], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+EB79 -> U+267B
  ["eebe87", :undef], # [BICYCLE] U+E4AE -> U+1F6B2
  ["eeb5b6", :undef], # [PEDESTRIAN] U+EB72 -> U+1F6B6
  ["eeb18b", :undef], # [BATH] U+E5D8 -> U+1F6C0
  ["eebdbd", :undef], # [RESTROOM] U+E4A5 -> U+1F6BB
  ["ef819d", :undef], # [NO ENTRY SIGN] U+E541 -> U+1F6AB
  ["ef81b3", "e29c94"], # [HEAVY CHECK MARK] U+E557 -> U+2714
  ["ef8388", :undef], # [SQUARED CL] U+E5AB -> U+1F191
  ["eeb198", :undef], # [SQUARED COOL] U+EA85 -> U+1F192
  ["ef8295", :undef], # [SQUARED FREE] U+E578 -> U+1F193
  ["eeb19b", :undef], # [SQUARED ID] U+EA88 -> U+1F194
  ["ef83a5", :undef], # [SQUARED NEW] U+E5B5 -> U+1F195
  ["ef838a", :undef], # [SQUARED OK] U+E5AD -> U+1F197
  ["eebf81", :undef], # [SQUARED SOS] U+E4E8 -> U+1F198
  ["eebfa8", :undef], # [SQUARED UP WITH EXCLAMATION MARK] U+E50F -> U+1F199
  ["eeb185", :undef], # [SQUARED VS] U+E5D2 -> U+1F19A
  ["eeb19a", :undef], # [SQUARED KATAKANA SA] U+EA87 -> U+1F202
  ["eeb19d", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+EA8A -> U+1F233
  ["eeb19c", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+EA89 -> U+1F235
  ["eeb199", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+EA86 -> U+1F239
  ["eeb19e", "f09f88af"], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+EA8B -> U+1F22F
  ["eeb19f", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+EA8C -> U+1F23A
  ["eebf8a", "e38a99"], # [CIRCLED IDEOGRAPH SECRET] U+E4F1 -> U+3299
  ["eeb1ac", "e38a97"], # [CIRCLED IDEOGRAPH CONGRATULATION] U+EA99 -> U+3297
  ["eebf90", :undef], # [CIRCLED IDEOGRAPH ADVANTAGE] U+E4F7 -> U+1F250
  ["ef8398", :undef], # [CIRCLED IDEOGRAPH ACCEPT] U+EB01 -> U+1F251
  ["ef8198", :undef], # [HEAVY PLUS SIGN] U+E53C -> U+2795
  ["ef8199", :undef], # [HEAVY MINUS SIGN] U+E53D -> U+2796
  ["ef81ab", "e29c96"], # [HEAVY MULTIPLICATION X] U+E54F -> U+2716
  ["ef81b0", :undef], # [HEAVY DIVISION SIGN] U+E554 -> U+2797
  ["eebd8e", :undef], # [ELECTRIC LIGHT BULB] U+E476 -> U+1F4A1
  ["eebebe", :undef], # [ANGER SYMBOL] U+E4E5 -> U+1F4A2
  ["eebd92", :undef], # [BOMB] U+E47A -> U+1F4A3
  ["eebd8d", :undef], # [SLEEPING SYMBOL] U+E475 -> U+1F4A4
  ["ef838d", :undef], # [COLLISION SYMBOL] U+E5B0 -> U+1F4A5
  ["ef838e", :undef], # [SPLASHING SWEAT SYMBOL] U+E5B1 -> U+1F4A6
  ["eebebf", :undef], # [DROP OF WATER] U+E4E6 -> U+1F4A7
  ["eebf8d", :undef], # [DASH SYMBOL] U+E4F4 -> U+1F4A8
  ["eebf8e", :undef], # [PILE OF POO] U+E4F5 -> U+1F4A9
  ["eebf82", :undef], # [FLEXED BICEPS] U+E4E9 -> U+1F4AA
  ["eeb5a0", :undef], # [DIZZY SYMBOL] U+EB5C -> U+1F4AB
  ["eebf96", :undef], # [SPEECH BALLOON] U+E4FD -> U+1F4AC
  ["eeb1be", :undef], # [SPARKLES] U+EAAB -> U+2728
  ["eebd91", "e29cb4"], # [EIGHT POINTED BLACK STAR] U+E479 -> U+2734
  ["ef819a", "e29cb3"], # [EIGHT SPOKED ASTERISK] U+E53E -> U+2733
  ["ef8196", "e29aaa"], # [MEDIUM WHITE CIRCLE] U+E53A -> U+26AA
  ["ef8197", "e29aab"], # [MEDIUM BLACK CIRCLE] U+E53B -> U+26AB
  ["ef81a6", :undef], # [LARGE RED CIRCLE] U+E54A -> U+1F534
  ["ef81a7", :undef], # [LARGE BLUE CIRCLE] U+E54B -> U+1F535
  ["eebda3", "e2ad90"], # [WHITE MEDIUM STAR] U+E48B -> U+2B50
  ["ef81a4", "e2ac9c"], # [WHITE LARGE SQUARE] U+E548 -> U+2B1C
  ["ef81a5", "e2ac9b"], # [BLACK LARGE SQUARE] U+E549 -> U+2B1B
  ["ef818d", "e296ab"], # [WHITE SMALL SQUARE] U+E531 -> U+25AB
  ["ef818e", "e296aa"], # [BLACK SMALL SQUARE] U+E532 -> U+25AA
  ["ef8190", "e297bd"], # [WHITE MEDIUM SMALL SQUARE] U+E534 -> U+25FD
  ["ef8191", "e297be"], # [BLACK MEDIUM SMALL SQUARE] U+E535 -> U+25FE
  ["ef8194", "e297bb"], # [WHITE MEDIUM SQUARE] U+E538 -> U+25FB
  ["ef8195", "e297bc"], # [BLACK MEDIUM SQUARE] U+E539 -> U+25FC
  ["ef81a2", :undef], # [LARGE ORANGE DIAMOND] U+E546 -> U+1F536
  ["ef81a3", :undef], # [LARGE BLUE DIAMOND] U+E547 -> U+1F537
  ["ef8192", :undef], # [SMALL ORANGE DIAMOND] U+E536 -> U+1F538
  ["ef8193", :undef], # [SMALL BLUE DIAMOND] U+E537 -> U+1F539
  ["eebd84", "e29d87"], # [SPARKLE] U+E46C -> U+2747
  ["eebf89", :undef], # [WHITE FLOWER] U+E4F0 -> U+1F4AE
  ["eebf8b", :undef], # [HUNDRED POINTS SYMBOL] U+E4F2 -> U+1F4AF
  ["ef81b9", "e286a9"], # [LEFTWARDS ARROW WITH HOOK] U+E55D -> U+21A9
  ["ef81b8", "e286aa"], # [RIGHTWARDS ARROW WITH HOOK] U+E55C -> U+21AA
  ["ef83a4", :undef], # [CLOCKWISE DOWNWARDS AND UPWARDS OPEN CIRCLE ARROWS] U+EB0D -> U+1F503
  ["eebfaa", :undef], # [SPEAKER WITH THREE SOUND WAVES] U+E511 -> U+1F50A
  ["ef82a1", :undef], # [BATTERY] U+E584 -> U+1F50B
  ["ef82a6", :undef], # [ELECTRIC PLUG] U+E589 -> U+1F50C
  ["eebfb1", :undef], # [LEFT-POINTING MAGNIFYING GLASS] U+E518 -> U+1F50D
  ["ef839c", :undef], # [RIGHT-POINTING MAGNIFYING GLASS] U+EB05 -> U+1F50E
  ["eebfb5", :undef], # [LOCK] U+E51C -> U+1F512
  ["ef83a3", :undef], # [LOCK WITH INK PEN] U+EB0C -> U+1F50F
  ["ef8393", :undef], # [CLOSED LOCK WITH KEY] U+EAFC -> U+1F510
  ["eebfb2", :undef], # [KEY] U+E519 -> U+1F511
  ["eebfab", :undef], # [BELL] U+E512 -> U+1F514
  ["ef8399", "e29891"], # [BALLOT BOX WITH CHECK] U+EB02 -> U+2611
  ["ef839b", :undef], # [RADIO BUTTON] U+EB04 -> U+1F518
  ["ef839e", :undef], # [BOOKMARK] U+EB07 -> U+1F516
  ["ef82a7", :undef], # [LINK SYMBOL] U+E58A -> U+1F517
  ["ef839d", :undef], # [BACK WITH LEFTWARDS ARROW ABOVE] U+EB06 -> U+1F519
  ["ef82a9", "e28083"], # [EM SPACE] U+E58C -> U+2003
  ["ef82aa", "e28082"], # [EN SPACE] U+E58D -> U+2002
  ["ef82ab", "e28085"], # [FOUR-PER-EM SPACE] U+E58E -> U+2005
  ["ef81ba", :undef], # [WHITE HEAVY CHECK MARK] U+E55E -> U+2705
  ["eeb688", :undef], # [RAISED FIST] U+EB83 -> U+270A
  ["ef8384", :undef], # [RAISED HAND] U+E5A7 -> U+270B
  ["ef8383", "e29c8c"], # [VICTORY HAND] U+E5A6 -> U+270C
  ["eebf8c", :undef], # [FISTED HAND SIGN] U+E4F3 -> U+1F44A
  ["eebf92", :undef], # [THUMBS UP SIGN] U+E4F9 -> U+1F44D
  ["eebf8f", "e2989d"], # [WHITE UP POINTING INDEX] U+E4F6 -> U+261D
  ["eeb1a0", :undef], # [WHITE UP POINTING BACKHAND INDEX] U+EA8D -> U+1F446
  ["eeb1a1", :undef], # [WHITE DOWN POINTING BACKHAND INDEX] U+EA8E -> U+1F447
  ["eebf98", :undef], # [WHITE LEFT POINTING BACKHAND INDEX] U+E4FF -> U+1F448
  ["eebf99", :undef], # [WHITE RIGHT POINTING BACKHAND INDEX] U+E500 -> U+1F449
  ["eeb2aa", :undef], # [WAVING HAND SIGN] U+EAD6 -> U+1F44B
  ["eeb2a7", :undef], # [CLAPPING HANDS SIGN] U+EAD3 -> U+1F44F
  ["eeb2a8", :undef], # [OK HAND SIGN] U+EAD4 -> U+1F44C
  ["eeb2a9", :undef], # [THUMBS DOWN SIGN] U+EAD5 -> U+1F44E
  ["ef8294", :undef], # [EMOJI COMPATIBILITY SYMBOL-37] U+E577 -> "[EZ]"
  ["ef838f", :undef], # [EMOJI COMPATIBILITY SYMBOL-38] U+E5B2 -> "[ezplus]"
  ["eeb1b0", :undef], # [EMOJI COMPATIBILITY SYMBOL-39] U+EA9D -> "[EZナビ]"
  ["eeb5b8", :undef], # [EMOJI COMPATIBILITY SYMBOL-40] U+EB74 -> "[EZムービー]"
  ["eeb686", :undef], # [EMOJI COMPATIBILITY SYMBOL-41] U+EB81 -> "[Cメール]"
  ["eeb68e", :undef], # [EMOJI COMPATIBILITY SYMBOL-42] U+EB89 -> "[Java]"
  ["eeb68f", :undef], # [EMOJI COMPATIBILITY SYMBOL-43] U+EB8A -> "[BREW]"
  ["eeb690", :undef], # [EMOJI COMPATIBILITY SYMBOL-44] U+EB8B -> "[EZ着うた]"
  ["eeb691", :undef], # [EMOJI COMPATIBILITY SYMBOL-45] U+EB8C -> "[EZナビ]"
  ["eeb692", :undef], # [EMOJI COMPATIBILITY SYMBOL-46] U+EB8D -> "[WIN]"
  ["eeb693", :undef], # [EMOJI COMPATIBILITY SYMBOL-47] U+EB8E -> "[プレミアム]"
  ["ef8392", :undef], # [EMOJI COMPATIBILITY SYMBOL-48] U+EAFB -> "[オープンウェブ]"
  ["ef82a3", :undef], # [EMOJI COMPATIBILITY SYMBOL-49] U+E586 -> "[PDC]"
  ["ef8188", :undef], # [EMOJI COMPATIBILITY SYMBOL-66] U+E52C -> "[Q]"
]

EMOJI_EXCHANGE_TBL['UTF8-SoftBank']['UTF8-DoCoMo'] = [
  ["ee818a", "ee98be"], # [BLACK SUN WITH RAYS] U+E04A -> U+E63E
  ["ee8189", "ee98bf"], # [CLOUD] U+E049 -> U+E63F
  ["ee818b", "ee9980"], # [UMBRELLA WITH RAIN DROPS] U+E04B -> U+E640
  ["ee8188", "ee9981"], # [SNOWMAN WITHOUT SNOW] U+E048 -> U+E641
  ["ee84bd", "ee9982"], # [HIGH VOLTAGE SIGN] U+E13D -> U+E642
  ["ee9183", "ee9983"], # [CYCLONE] U+E443 -> U+E643
  ["ee90bc", "ee9985"], # [CLOSED UMBRELLA] U+E43C -> U+E645
  ["ee918b", "ee9ab3"], # [NIGHT WITH STARS] U+E44B -> U+E6B3
  ["ee818d", "ee98be"], # [SUNRISE OVER MOUNTAINS] U+E04D -> U+E63E
  ["ee9189", "ee98be"], # [SUNRISE] U+E449 -> U+E63E
  ["ee8586", :undef], # [CITYSCAPE AT DUSK] U+E146 -> "[夕焼け]"
  ["ee918a", "ee98be"], # [SUNSET OVER BUILDINGS] U+E44A -> U+E63E
  ["ee918c", :undef], # [RAINBOW] U+E44C -> "[虹]"
  ["ee90be", "ee9cbf"], # [WATER WAVE] U+E43E -> U+E73F
  ["ee818c", "ee9a9f"], # [CRESCENT MOON] U+E04C -> U+E69F
  ["ee8cb5", :undef], # [GLOWING STAR] U+E335 -> "[☆]"
  ["ee80a4", "ee9aba"], # [CLOCK FACE ONE OCLOCK] U+E024 -> U+E6BA
  ["ee80a5", "ee9aba"], # [CLOCK FACE TWO OCLOCK] U+E025 -> U+E6BA
  ["ee80a6", "ee9aba"], # [CLOCK FACE THREE OCLOCK] U+E026 -> U+E6BA
  ["ee80a7", "ee9aba"], # [CLOCK FACE FOUR OCLOCK] U+E027 -> U+E6BA
  ["ee80a8", "ee9aba"], # [CLOCK FACE FIVE OCLOCK] U+E028 -> U+E6BA
  ["ee80a9", "ee9aba"], # [CLOCK FACE SIX OCLOCK] U+E029 -> U+E6BA
  ["ee80aa", "ee9aba"], # [CLOCK FACE SEVEN OCLOCK] U+E02A -> U+E6BA
  ["ee80ab", "ee9aba"], # [CLOCK FACE EIGHT OCLOCK] U+E02B -> U+E6BA
  ["ee80ac", "ee9aba"], # [CLOCK FACE NINE OCLOCK] U+E02C -> U+E6BA
  ["ee80ad", "ee9aba"], # [CLOCK FACE TEN OCLOCK] U+E02D -> U+E6BA
  ["ee80ae", "ee9aba"], # [CLOCK FACE ELEVEN OCLOCK] U+E02E -> U+E6BA
  ["ee80af", "ee9aba"], # [CLOCK FACE TWELVE OCLOCK] U+E02F -> U+E6BA
  ["ee88bf", "ee9986"], # [ARIES] U+E23F -> U+E646
  ["ee8980", "ee9987"], # [TAURUS] U+E240 -> U+E647
  ["ee8981", "ee9988"], # [GEMINI] U+E241 -> U+E648
  ["ee8982", "ee9989"], # [CANCER] U+E242 -> U+E649
  ["ee8983", "ee998a"], # [LEO] U+E243 -> U+E64A
  ["ee8984", "ee998b"], # [VIRGO] U+E244 -> U+E64B
  ["ee8985", "ee998c"], # [LIBRA] U+E245 -> U+E64C
  ["ee8986", "ee998d"], # [SCORPIUS] U+E246 -> U+E64D
  ["ee8987", "ee998e"], # [SAGITTARIUS] U+E247 -> U+E64E
  ["ee8988", "ee998f"], # [CAPRICORN] U+E248 -> U+E64F
  ["ee8989", "ee9990"], # [AQUARIUS] U+E249 -> U+E650
  ["ee898a", "ee9991"], # [PISCES] U+E24A -> U+E651
  ["ee898b", :undef], # [OPHIUCHUS] U+E24B -> "[蛇使座]"
  ["ee8490", "ee9d81"], # [FOUR LEAF CLOVER] U+E110 -> U+E741
  ["ee8c84", "ee9d83"], # [TULIP] U+E304 -> U+E743
  ["ee8498", "ee9d87"], # [MAPLE LEAF] U+E118 -> U+E747
  ["ee80b0", "ee9d88"], # [CHERRY BLOSSOM] U+E030 -> U+E748
  ["ee80b2", :undef], # [ROSE] U+E032 -> "[バラ]"
  ["ee8499", "ee9d87"], # [FALLEN LEAF] U+E119 -> U+E747
  ["ee9187", :undef], # [LEAF FLUTTERING IN WIND] U+E447 -> "[風に舞う葉]"
  ["ee8c83", :undef], # [HIBISCUS] U+E303 -> "[ハイビスカス]"
  ["ee8c85", :undef], # [SUNFLOWER] U+E305 -> "[ひまわり]"
  ["ee8c87", :undef], # [PALM TREE] U+E307 -> "[ヤシ]"
  ["ee8c88", :undef], # [CACTUS] U+E308 -> "[サボテン]"
  ["ee9184", :undef], # [EAR OF RICE] U+E444 -> "[稲穂]"
  ["ee8d85", "ee9d85"], # [RED APPLE] U+E345 -> U+E745
  ["ee8d86", :undef], # [TANGERINE] U+E346 -> "[みかん]"
  ["ee8d87", :undef], # [STRAWBERRY] U+E347 -> "[イチゴ]"
  ["ee8d88", :undef], # [WATERMELON] U+E348 -> "[スイカ]"
  ["ee8d89", :undef], # [TOMATO] U+E349 -> "[トマト]"
  ["ee8d8a", :undef], # [AUBERGINE] U+E34A -> "[ナス]"
  ["ee9099", "ee9a91"], # [EYES] U+E419 -> U+E691
  ["ee909b", "ee9a92"], # [EAR] U+E41B -> U+E692
  ["ee909a", :undef], # [NOSE] U+E41A -> "[鼻]"
  ["ee909c", "ee9bb9"], # [MOUTH] U+E41C -> U+E6F9
  ["ee8c9c", "ee9c90"], # [LIPSTICK] U+E31C -> U+E710
  ["ee8c9d", :undef], # [NAIL POLISH] U+E31D -> "[マニキュア]"
  ["ee8c9e", :undef], # [FACE MASSAGE] U+E31E -> "[エステ]"
  ["ee8c9f", "ee99b5"], # [HAIRCUT] U+E31F -> U+E675
  ["ee8ca0", :undef], # [BARBER POLE] U+E320 -> "[床屋]"
  ["ee8081", "ee9bb0"], # [BOY] U+E001 -> U+E6F0
  ["ee8082", "ee9bb0"], # [GIRL] U+E002 -> U+E6F0
  ["ee8084", "ee9bb0"], # [MAN] U+E004 -> U+E6F0
  ["ee8085", "ee9bb0"], # [WOMAN] U+E005 -> U+E6F0
  ["ee90a8", :undef], # [MAN AND WOMAN HOLDING HANDS] U+E428 -> "[カップル]"
  ["ee8592", :undef], # [POLICE OFFICER] U+E152 -> "[警官]"
  ["ee90a9", :undef], # [WOMAN WITH BUNNY EARS] U+E429 -> "[バニー]"
  ["ee9495", :undef], # [WESTERN PERSON] U+E515 -> "[白人]"
  ["ee9496", :undef], # [MAN WITH GUA PI MAO] U+E516 -> "[中国人]"
  ["ee9497", :undef], # [MAN WITH TURBAN] U+E517 -> "[インド人]"
  ["ee9498", :undef], # [OLDER MAN] U+E518 -> "[おじいさん]"
  ["ee9499", :undef], # [OLDER WOMAN] U+E519 -> "[おばあさん]"
  ["ee949a", :undef], # [BABY] U+E51A -> "[赤ちゃん]"
  ["ee949b", :undef], # [CONSTRUCTION WORKER] U+E51B -> "[工事現場の人]"
  ["ee949c", :undef], # [PRINCESS] U+E51C -> "[お姫様]"
  ["ee849b", :undef], # [GHOST] U+E11B -> "[お化け]"
  ["ee818e", :undef], # [BABY ANGEL] U+E04E -> "[天使]"
  ["ee848c", :undef], # [EXTRATERRESTRIAL ALIEN] U+E10C -> "[UFO]"
  ["ee84ab", :undef], # [ALIEN MONSTER] U+E12B -> "[宇宙人]"
  ["ee849a", :undef], # [IMP] U+E11A -> "[アクマ]"
  ["ee849c", :undef], # [SKULL] U+E11C -> "[ドクロ]"
  ["ee8993", :undef], # [INFORMATION DESK PERSON] U+E253 -> "[案内]"
  ["ee949e", :undef], # [GUARDSMAN] U+E51E -> "[衛兵]"
  ["ee949f", :undef], # [DANCER] U+E51F -> "[ダンス]"
  ["ee94ad", :undef], # [SNAKE] U+E52D -> "[ヘビ]"
  ["ee84b4", "ee9d94"], # [HORSE] U+E134 -> U+E754
  ["ee94ae", :undef], # [CHICKEN] U+E52E -> "[ニワトリ]"
  ["ee94af", :undef], # [BOAR] U+E52F -> "[イノシシ]"
  ["ee94b0", :undef], # [BACTRIAN CAMEL] U+E530 -> "[ラクダ]"
  ["ee94a6", :undef], # [ELEPHANT] U+E526 -> "[ゾウ]"
  ["ee94a7", :undef], # [KOALA] U+E527 -> "[コアラ]"
  ["ee94a8", :undef], # [MONKEY] U+E528 -> "[サル]"
  ["ee94a9", :undef], # [SHEEP] U+E529 -> "[ヒツジ]"
  ["ee848a", :undef], # [OCTOPUS] U+E10A -> "[タコ]"
  ["ee9181", :undef], # [SPIRAL SHELL] U+E441 -> "[巻貝]"
  ["ee94a5", :undef], # [BUG] U+E525 -> "[ゲジゲジ]"
  ["ee94a2", "ee9d91"], # [TROPICAL FISH] U+E522 -> U+E751
  ["ee94a3", "ee9d8f"], # [BABY CHICK] U+E523 -> U+E74F
  ["ee94a1", "ee9d8f"], # [BIRD] U+E521 -> U+E74F
  ["ee8195", "ee9d90"], # [PENGUIN] U+E055 -> U+E750
  ["ee8099", "ee9d91"], # [FISH] U+E019 -> U+E751
  ["ee94a0", :undef], # [DOLPHIN] U+E520 -> "[イルカ]"
  ["ee8193", :undef], # [MOUSE FACE] U+E053 -> "[ネズミ]"
  ["ee8190", :undef], # [TIGER FACE] U+E050 -> "[トラ]"
  ["ee818f", "ee9aa2"], # [CAT FACE] U+E04F -> U+E6A2
  ["ee8194", :undef], # [SPOUTING WHALE] U+E054 -> "[クジラ]"
  ["ee809a", "ee9d94"], # [HORSE FACE] U+E01A -> U+E754
  ["ee8489", :undef], # [MONKEY FACE] U+E109 -> "[サル]"
  ["ee8192", "ee9aa1"], # [DOG FACE] U+E052 -> U+E6A1
  ["ee848b", "ee9d95"], # [PIG FACE] U+E10B -> U+E755
  ["ee8191", :undef], # [BEAR FACE] U+E051 -> "[クマ]"
  ["ee94a4", :undef], # [HAMSTER FACE] U+E524 -> "[ハムスター]"
  ["ee94aa", "ee9aa1"], # [WOLF FACE] U+E52A -> U+E6A1
  ["ee94ab", :undef], # [COW FACE] U+E52B -> "[牛]"
  ["ee94ac", :undef], # [RABBIT FACE] U+E52C -> "[ウサギ]"
  ["ee94b1", :undef], # [FROG FACE] U+E531 -> "[カエル]"
  ["ee8199", "ee9bb1"], # [ANGRY FACE] U+E059 -> U+E6F1
  ["ee9090", "ee9bb4"], # [ASTONISHED FACE] U+E410 -> U+E6F4
  ["ee8198", "ee9bb2"], # [DISAPPOINTED FACE] U+E058 -> U+E6F2
  ["ee908f", "ee9ca3"], # [EXASPERATED FACE] U+E40F -> U+E723
  ["ee908e", "ee9ca5"], # [EXPRESSIONLESS FACE] U+E40E -> U+E725
  ["ee8486", "ee9ca6"], # [FACE WITH HEART-SHAPED EYES] U+E106 -> U+E726
  ["ee8485", "ee9ca8"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E105 -> U+E728
  ["ee9089", "ee9ca8"], # [FACE WITH STUCK-OUT TONGUE] U+E409 -> U+E728
  ["ee9098", "ee9ca6"], # [FACE THROWING A KISS] U+E418 -> U+E726
  ["ee9097", "ee9ca6"], # [FACE KISSING] U+E417 -> U+E726
  ["ee908c", :undef], # [FACE WITH MASK] U+E40C -> "[風邪ひき]"
  ["ee908d", "ee9caa"], # [FLUSHED FACE] U+E40D -> U+E72A
  ["ee8197", "ee9bb0"], # [HAPPY FACE WITH OPEN MOUTH] U+E057 -> U+E6F0
  ["ee9084", "ee9d93"], # [HAPPY FACE WITH GRIN] U+E404 -> U+E753
  ["ee9092", "ee9caa"], # [HAPPY AND CRYING FACE] U+E412 -> U+E72A
  ["ee8196", "ee9bb0"], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+E056 -> U+E6F0
  ["ee9094", "ee9bb0"], # [WHITE SMILING FACE] U+E414 -> U+E6F0
  ["ee9095", "ee9bb0"], # [HAPPY FACE WITH OPEN MOUTH AND RAISED EYEBROWS] U+E415 -> U+E6F0
  ["ee9093", "ee9cae"], # [CRYING FACE] U+E413 -> U+E72E
  ["ee9091", "ee9cad"], # [LOUDLY CRYING FACE] U+E411 -> U+E72D
  ["ee908b", "ee9d97"], # [FEARFUL FACE] U+E40B -> U+E757
  ["ee9086", "ee9cab"], # [PERSEVERING FACE] U+E406 -> U+E72B
  ["ee9096", "ee9ca4"], # [POUTING FACE] U+E416 -> U+E724
  ["ee908a", "ee9ca1"], # [RELIEVED FACE] U+E40A -> U+E721
  ["ee9087", "ee9bb3"], # [CONFOUNDED FACE] U+E407 -> U+E6F3
  ["ee9083", "ee9ca0"], # [PENSIVE FACE] U+E403 -> U+E720
  ["ee8487", "ee9d97"], # [FACE SCREAMING IN FEAR] U+E107 -> U+E757
  ["ee9088", "ee9c81"], # [SLEEPY FACE] U+E408 -> U+E701
  ["ee9082", "ee9cac"], # [SMIRKING FACE] U+E402 -> U+E72C
  ["ee8488", "ee9ca3"], # [FACE WITH COLD SWEAT] U+E108 -> U+E723
  ["ee9081", "ee9ca3"], # [DISAPPOINTED BUT RELIEVED FACE] U+E401 -> U+E723
  ["ee9085", "ee9ca9"], # [WINKING FACE] U+E405 -> U+E729
  ["ee90a3", "ee9caf"], # [FACE WITH NO GOOD GESTURE] U+E423 -> U+E72F
  ["ee90a4", "ee9c8b"], # [FACE WITH OK GESTURE] U+E424 -> U+E70B
  ["ee90a6", :undef], # [PERSON BOWING DEEPLY] U+E426 -> "m(_ _)m"
  ["ee90a7", :undef], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+E427 -> "＼(^o^)／"
  ["ee909d", :undef], # [PERSON WITH FOLDED HANDS] U+E41D -> "(&gt;人&lt;)"
  ["ee80b6", "ee99a3"], # [HOUSE BUILDING] U+E036 -> U+E663
  ["ee80b8", "ee99a4"], # [OFFICE BUILDING] U+E038 -> U+E664
  ["ee8593", "ee99a5"], # [JAPANESE POST OFFICE] U+E153 -> U+E665
  ["ee8595", "ee99a6"], # [HOSPITAL] U+E155 -> U+E666
  ["ee858d", "ee99a7"], # [BANK] U+E14D -> U+E667
  ["ee8594", "ee99a8"], # [AUTOMATED TELLER MACHINE] U+E154 -> U+E668
  ["ee8598", "ee99a9"], # [HOTEL] U+E158 -> U+E669
  ["ee9481", "ee99a9ee9baf"], # [LOVE HOTEL] U+E501 -> U+E669 U+E6EF
  ["ee8596", "ee99aa"], # [CONVENIENCE STORE] U+E156 -> U+E66A
  ["ee8597", "ee9cbe"], # [SCHOOL] U+E157 -> U+E73E
  ["ee80b7", :undef], # [CHURCH] U+E037 -> "[教会]"
  ["ee84a1", :undef], # [FOUNTAIN] U+E121 -> "[噴水]"
  ["ee9484", :undef], # [DEPARTMENT STORE] U+E504 -> "[デパート]"
  ["ee9485", :undef], # [JAPANESE CASTLE] U+E505 -> "[城]"
  ["ee9486", :undef], # [EUROPEAN CASTLE] U+E506 -> "[城]"
  ["ee9488", :undef], # [FACTORY] U+E508 -> "[工場]"
  ["ee80bb", "ee9d80"], # [MOUNT FUJI] U+E03B -> U+E740
  ["ee9489", :undef], # [TOKYO TOWER] U+E509 -> "[東京タワー]"
  ["ee949d", :undef], # [STATUE OF LIBERTY] U+E51D -> "[自由の女神]"
  ["ee8087", "ee9a99"], # [ATHLETIC SHOE] U+E007 -> U+E699
  ["ee84be", "ee99b4"], # [HIGH-HEELED SHOE] U+E13E -> U+E674
  ["ee8c9a", "ee99b4"], # [WOMANS SANDAL] U+E31A -> U+E674
  ["ee8c9b", :undef], # [WOMANS BOOTS] U+E31B -> "[ブーツ]"
  ["ee94b6", "ee9a98"], # [FOOTPRINTS] U+E536 -> U+E698
  ["ee8086", "ee9c8e"], # [T-SHIRT] U+E006 -> U+E70E
  ["ee848e", "ee9c9a"], # [CROWN] U+E10E -> U+E71A
  ["ee8c82", :undef], # [NECKTIE] U+E302 -> "[ネクタイ]"
  ["ee8c98", :undef], # [WOMANS HAT] U+E318 -> "[帽子]"
  ["ee8c99", :undef], # [DRESS] U+E319 -> "[ドレス]"
  ["ee8ca1", :undef], # [KIMONO] U+E321 -> "[着物]"
  ["ee8ca2", :undef], # [BIKINI] U+E322 -> "[ビキニ]"
  ["ee8ca3", "ee9a82"], # [HANDBAG] U+E323 -> U+E682
  ["ee84af", "ee9c95"], # [MONEY BAG] U+E12F -> U+E715
  ["ee8589", :undef], # [CURRENCY EXCHANGE] U+E149 -> "[$￥]"
  ["ee858a", :undef], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+E14A -> "[株価]"
  ["ee9493", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS CN] U+E513 -> "[中国]"
  ["ee948e", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS DE] U+E50E -> "[ドイツ]"
  ["ee9491", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS ES] U+E511 -> "[スペイン]"
  ["ee948d", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS FR] U+E50D -> "[フランス]"
  ["ee9490", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS GB] U+E510 -> "[イギリス]"
  ["ee948f", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS IT] U+E50F -> "[イタリア]"
  ["ee948b", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS JP] U+E50B -> "[日の丸]"
  ["ee9494", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS KR] U+E514 -> "[韓国]"
  ["ee9492", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS RU] U+E512 -> "[ロシア]"
  ["ee948c", :undef], # [REGIONAL INDICATOR SYMBOL LETTERS US] U+E50C -> "[USA]"
  ["ee849d", :undef], # [FIRE] U+E11D -> "[炎]"
  ["ee8496", :undef], # [HAMMER] U+E116 -> "[ハンマー]"
  ["ee8493", :undef], # [PISTOL] U+E113 -> "[ピストル]"
  ["ee88be", :undef], # [SIX POINTED STAR WITH MIDDLE DOT] U+E23E -> "[占い]"
  ["ee8889", :undef], # [JAPANESE SYMBOL FOR BEGINNER] U+E209 -> "[若葉マーク]"
  ["ee80b1", "ee9c9a"], # [TRIDENT EMBLEM] U+E031 -> U+E71A
  ["ee84bb", :undef], # [SYRINGE] U+E13B -> "[注射]"
  ["ee8c8f", :undef], # [PILL] U+E30F -> "[薬]"
  ["ee94b2", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+E532 -> "[A]"
  ["ee94b3", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+E533 -> "[B]"
  ["ee94b4", :undef], # [NEGATIVE SQUARED AB] U+E534 -> "[AB]"
  ["ee94b5", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+E535 -> "[O]"
  ["ee8c94", "ee9a84"], # [RIBBON] U+E314 -> U+E684
  ["ee8492", "ee9a85"], # [WRAPPED PRESENT] U+E112 -> U+E685
  ["ee8d8b", "ee9a86"], # [BIRTHDAY CAKE] U+E34B -> U+E686
  ["ee80b3", "ee9aa4"], # [CHRISTMAS TREE] U+E033 -> U+E6A4
  ["ee9188", :undef], # [FATHER CHRISTMAS] U+E448 -> "[サンタ]"
  ["ee8583", :undef], # [CROSSED FLAGS] U+E143 -> "[祝日]"
  ["ee8497", :undef], # [FIREWORKS] U+E117 -> "[花火]"
  ["ee8c90", :undef], # [BALLOON] U+E310 -> "[風船]"
  ["ee8c92", :undef], # [PARTY POPPER] U+E312 -> "[クラッカー]"
  ["ee90b6", :undef], # [PINE DECORATION] U+E436 -> "[門松]"
  ["ee90b8", :undef], # [JAPANESE DOLLS] U+E438 -> "[ひな祭り]"
  ["ee90b9", :undef], # [GRADUATION CAP] U+E439 -> "[卒業式]"
  ["ee90ba", :undef], # [SCHOOL SATCHEL] U+E43A -> "[ランドセル]"
  ["ee90bb", :undef], # [CARP STREAMER] U+E43B -> "[こいのぼり]"
  ["ee9180", :undef], # [FIREWORK SPARKLER] U+E440 -> "[線香花火]"
  ["ee9182", :undef], # [WIND CHIME] U+E442 -> "[風鈴]"
  ["ee9185", :undef], # [JACK-O-LANTERN] U+E445 -> "[ハロウィン]"
  ["ee9186", :undef], # [MOON VIEWING CEREMONY] U+E446 -> "[お月見]"
  ["ee8089", "ee9a87"], # [BLACK TELEPHONE] U+E009 -> U+E687
  ["ee808a", "ee9a88"], # [MOBILE PHONE] U+E00A -> U+E688
  ["ee8484", "ee9b8e"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+E104 -> U+E6CE
  ["ee8c81", "ee9a89"], # [MEMO] U+E301 -> U+E689
  ["ee808b", "ee9b90"], # [FAX MACHINE] U+E00B -> U+E6D0
  ["ee8483", "ee9b8f"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+E103 -> U+E6CF
  ["ee8481", "ee99a5"], # [CLOSED MAILBOX WITH RAISED FLAG] U+E101 -> U+E665
  ["ee8482", "ee99a5"], # [POSTBOX] U+E102 -> U+E665
  ["ee8582", :undef], # [PUBLIC ADDRESS LOUDSPEAKER] U+E142 -> "[スピーカ]"
  ["ee8c97", :undef], # [CHEERING MEGAPHONE] U+E317 -> "[メガホン]"
  ["ee858b", :undef], # [SATELLITE ANTENNA] U+E14B -> "[アンテナ]"
  ["ee849f", "ee9ab2"], # [SEAT] U+E11F -> U+E6B2
  ["ee808c", "ee9c96"], # [PERSONAL COMPUTER] U+E00C -> U+E716
  ["ee849e", "ee9a82"], # [BRIEFCASE] U+E11E -> U+E682
  ["ee8c96", :undef], # [MINIDISC] U+E316 -> "[MD]"
  ["ee84a6", "ee9a8c"], # [OPTICAL DISC] U+E126 -> U+E68C
  ["ee84a7", "ee9a8c"], # [DVD] U+E127 -> U+E68C
  ["ee8c93", "ee99b5"], # [BLACK SCISSORS] U+E313 -> U+E675
  ["ee8588", "ee9a83"], # [OPEN BOOK] U+E148 -> U+E683
  ["ee8096", "ee9993"], # [BASEBALL] U+E016 -> U+E653
  ["ee8094", "ee9994"], # [FLAG IN HOLE] U+E014 -> U+E654
  ["ee8095", "ee9995"], # [TENNIS RACQUET AND BALL] U+E015 -> U+E655
  ["ee8098", "ee9996"], # [SOCCER BALL] U+E018 -> U+E656
  ["ee8093", "ee9997"], # [SKI AND SKI BOOT] U+E013 -> U+E657
  ["ee90aa", "ee9998"], # [BASKETBALL AND HOOP] U+E42A -> U+E658
  ["ee84b2", "ee9999"], # [CHEQUERED FLAG] U+E132 -> U+E659
  ["ee8495", "ee9cb3"], # [RUNNER] U+E115 -> U+E733
  ["ee8097", "ee9c92"], # [SURFER] U+E017 -> U+E712
  ["ee84b1", :undef], # [TROPHY] U+E131 -> "[トロフィー]"
  ["ee90ab", :undef], # [AMERICAN FOOTBALL] U+E42B -> "[フットボール]"
  ["ee90ad", :undef], # [SWIMMER] U+E42D -> "[水泳]"
  ["ee809e", "ee999b"], # [TRAIN] U+E01E -> U+E65B
  ["ee90b4", "ee999c"], # [METRO] U+E434 -> U+E65C
  ["ee90b5", "ee999d"], # [HIGH-SPEED TRAIN] U+E435 -> U+E65D
  ["ee809f", "ee999d"], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+E01F -> U+E65D
  ["ee809b", "ee999e"], # [AUTOMOBILE] U+E01B -> U+E65E
  ["ee90ae", "ee999f"], # [RECREATIONAL VEHICLE] U+E42E -> U+E65F
  ["ee8599", "ee99a0"], # [ONCOMING BUS] U+E159 -> U+E660
  ["ee8590", :undef], # [BUS STOP] U+E150 -> "[バス停]"
  ["ee8882", "ee99a1"], # [SHIP] U+E202 -> U+E661
  ["ee809d", "ee99a2"], # [AIRPLANE] U+E01D -> U+E662
  ["ee809c", "ee9aa3"], # [SAILBOAT] U+E01C -> U+E6A3
  ["ee80b9", :undef], # [STATION] U+E039 -> "[駅]"
  ["ee848d", :undef], # [ROCKET] U+E10D -> "[ロケット]"
  ["ee84b5", "ee9aa3"], # [SPEEDBOAT] U+E135 -> U+E6A3
  ["ee859a", "ee999e"], # [TAXI] U+E15A -> U+E65E
  ["ee90af", :undef], # [DELIVERY TRUCK] U+E42F -> "[トラック]"
  ["ee90b0", :undef], # [FIRE ENGINE] U+E430 -> "[消防車]"
  ["ee90b1", :undef], # [AMBULANCE] U+E431 -> "[救急車]"
  ["ee90b2", :undef], # [POLICE CAR] U+E432 -> "[パトカー]"
  ["ee80ba", "ee99ab"], # [FUEL PUMP] U+E03A -> U+E66B
  ["ee858f", "ee99ac"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E14F -> U+E66C
  ["ee858e", "ee99ad"], # [HORIZONTAL TRAFFIC LIGHT] U+E14E -> U+E66D
  ["ee84b7", :undef], # [CONSTRUCTION SIGN] U+E137 -> "[工事中]"
  ["ee84a3", "ee9bb7"], # [HOT SPRINGS] U+E123 -> U+E6F7
  ["ee84a2", :undef], # [TENT] U+E122 -> "[キャンプ]"
  ["ee84a4", :undef], # [FERRIS WHEEL] U+E124 -> "[観覧車]"
  ["ee90b3", :undef], # [ROLLER COASTER] U+E433 -> "[ジェットコースター]"
  ["ee80bc", "ee99b6"], # [MICROPHONE] U+E03C -> U+E676
  ["ee80bd", "ee99b7"], # [MOVIE CAMERA] U+E03D -> U+E677
  ["ee9487", "ee99b7"], # [CINEMA] U+E507 -> U+E677
  ["ee8c8a", "ee99ba"], # [HEADPHONE] U+E30A -> U+E67A
  ["ee9482", "ee99bb"], # [ARTIST PALETTE] U+E502 -> U+E67B
  ["ee9483", "ee99bc"], # [TOP HAT] U+E503 -> U+E67C
  ["ee84a5", "ee99be"], # [TICKET] U+E125 -> U+E67E
  ["ee8ca4", "ee9aac"], # [CLAPPER BOARD] U+E324 -> U+E6AC
  ["ee84ad", :undef], # [MAHJONG TILE RED DRAGON] U+E12D -> "[麻雀]"
  ["ee84b0", :undef], # [DIRECT HIT] U+E130 -> "[的中]"
  ["ee84b3", :undef], # [SLOT MACHINE] U+E133 -> "[777]"
  ["ee90ac", :undef], # [BILLIARDS] U+E42C -> "[ビリヤード]"
  ["ee80be", "ee9bb6"], # [MUSICAL NOTE] U+E03E -> U+E6F6
  ["ee8ca6", "ee9bbf"], # [MULTIPLE MUSICAL NOTES] U+E326 -> U+E6FF
  ["ee8180", :undef], # [SAXOPHONE] U+E040 -> "[サックス]"
  ["ee8181", :undef], # [GUITAR] U+E041 -> "[ギター]"
  ["ee8182", :undef], # [TRUMPET] U+E042 -> "[トランペット]"
  ["ee84ac", :undef], # [PART ALTERNATION MARK] U+E12C -> "[歌記号]"
  ["ee8088", "ee9a81"], # [CAMERA] U+E008 -> U+E681
  ["ee84aa", "ee9a8a"], # [TELEVISION] U+E12A -> U+E68A
  ["ee84a8", :undef], # [RADIO] U+E128 -> "[ラジオ]"
  ["ee84a9", :undef], # [VIDEOCASSETTE] U+E129 -> "[ビデオ]"
  ["ee8083", "ee9bb9"], # [KISS MARK] U+E003 -> U+E6F9
  ["ee80b4", "ee9c9b"], # [RING] U+E034 -> U+E71B
  ["ee80b5", "ee9c9b"], # [GEM STONE] U+E035 -> U+E71B
  ["ee8491", "ee9bb9"], # [KISS] U+E111 -> U+E6F9
  ["ee8c86", :undef], # [BOUQUET] U+E306 -> "[花束]"
  ["ee90a5", "ee9bad"], # [COUPLE WITH HEART] U+E425 -> U+E6ED
  ["ee90bd", :undef], # [WEDDING] U+E43D -> "[結婚式]"
  ["ee8887", :undef], # [NO ONE UNDER EIGHTEEN SYMBOL] U+E207 -> "[18禁]"
  ["ee898e", "ee9cb1"], # [COPYRIGHT SIGN] U+E24E -> U+E731
  ["ee898f", "ee9cb6"], # [REGISTERED SIGN] U+E24F -> U+E736
  ["ee94b7", "ee9cb2"], # [TRADE MARK SIGN] U+E537 -> U+E732
  ["ee8890", "ee9ba0"], # [HASH KEY] U+E210 -> U+E6E0
  ["ee889c", "ee9ba2"], # [KEYCAP 1] U+E21C -> U+E6E2
  ["ee889d", "ee9ba3"], # [KEYCAP 2] U+E21D -> U+E6E3
  ["ee889e", "ee9ba4"], # [KEYCAP 3] U+E21E -> U+E6E4
  ["ee889f", "ee9ba5"], # [KEYCAP 4] U+E21F -> U+E6E5
  ["ee88a0", "ee9ba6"], # [KEYCAP 5] U+E220 -> U+E6E6
  ["ee88a1", "ee9ba7"], # [KEYCAP 6] U+E221 -> U+E6E7
  ["ee88a2", "ee9ba8"], # [KEYCAP 7] U+E222 -> U+E6E8
  ["ee88a3", "ee9ba9"], # [KEYCAP 8] U+E223 -> U+E6E9
  ["ee88a4", "ee9baa"], # [KEYCAP 9] U+E224 -> U+E6EA
  ["ee88a5", "ee9bab"], # [KEYCAP 0] U+E225 -> U+E6EB
  ["ee888b", :undef], # [ANTENNA WITH BARS] U+E20B -> "[バリ3]"
  ["ee8990", :undef], # [VIBRATION MODE] U+E250 -> "[マナーモード]"
  ["ee8991", :undef], # [MOBILE PHONE OFF] U+E251 -> "[ケータイOFF]"
  ["ee84a0", "ee99b3"], # [HAMBURGER] U+E120 -> U+E673
  ["ee8d82", "ee9d89"], # [RICE BALL] U+E342 -> U+E749
  ["ee8186", "ee9d8a"], # [SHORTCAKE] U+E046 -> U+E74A
  ["ee8d80", "ee9d8c"], # [STEAMING BOWL] U+E340 -> U+E74C
  ["ee8cb9", "ee9d8d"], # [BREAD] U+E339 -> U+E74D
  ["ee8587", :undef], # [COOKING] U+E147 -> "[フライパン]"
  ["ee8cba", :undef], # [SOFT ICE CREAM] U+E33A -> "[ソフトクリーム]"
  ["ee8cbb", :undef], # [FRENCH FRIES] U+E33B -> "[ポテト]"
  ["ee8cbc", :undef], # [DANGO] U+E33C -> "[だんご]"
  ["ee8cbd", :undef], # [RICE CRACKER] U+E33D -> "[せんべい]"
  ["ee8cbe", "ee9d8c"], # [COOKED RICE] U+E33E -> U+E74C
  ["ee8cbf", :undef], # [SPAGHETTI] U+E33F -> "[パスタ]"
  ["ee8d81", :undef], # [CURRY AND RICE] U+E341 -> "[カレー]"
  ["ee8d83", :undef], # [ODEN] U+E343 -> "[おでん]"
  ["ee8d84", :undef], # [SUSHI] U+E344 -> "[すし]"
  ["ee8d8c", :undef], # [BENTO BOX] U+E34C -> "[弁当]"
  ["ee8d8d", :undef], # [POT OF FOOD] U+E34D -> "[鍋]"
  ["ee90bf", :undef], # [SHAVED ICE] U+E43F -> "[カキ氷]"
  ["ee8183", "ee99af"], # [FORK AND KNIFE] U+E043 -> U+E66F
  ["ee8185", "ee99b0"], # [HOT BEVERAGE] U+E045 -> U+E670
  ["ee8184", "ee99b1"], # [COCKTAIL GLASS] U+E044 -> U+E671
  ["ee8187", "ee99b2"], # [BEER MUG] U+E047 -> U+E672
  ["ee8cb8", "ee9c9e"], # [TEACUP WITHOUT HANDLE] U+E338 -> U+E71E
  ["ee8c8b", "ee9d8b"], # [SAKE BOTTLE AND CUP] U+E30B -> U+E74B
  ["ee8c8c", "ee99b2"], # [CLINKING BEER MUGS] U+E30C -> U+E672
  ["ee88b6", "ee99b8"], # [NORTH EAST ARROW] U+E236 -> U+E678
  ["ee88b8", "ee9a96"], # [SOUTH EAST ARROW] U+E238 -> U+E696
  ["ee88b7", "ee9a97"], # [NORTH WEST ARROW] U+E237 -> U+E697
  ["ee88b9", "ee9aa5"], # [SOUTH WEST ARROW] U+E239 -> U+E6A5
  ["ee88b2", :undef], # [UPWARDS BLACK ARROW] U+E232 -> "[↑]"
  ["ee88b3", :undef], # [DOWNWARDS BLACK ARROW] U+E233 -> "[↓]"
  ["ee88b4", :undef], # [BLACK RIGHTWARDS ARROW] U+E234 -> "[→]"
  ["ee88b5", :undef], # [LEFTWARDS BLACK ARROW] U+E235 -> "[←]"
  ["ee88ba", :undef], # [BLACK RIGHT-POINTING TRIANGLE] U+E23A -> "[&gt;]"
  ["ee88bb", :undef], # [BLACK LEFT-POINTING TRIANGLE] U+E23B -> "[&lt;]"
  ["ee88bc", :undef], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+E23C -> "[&gt;&gt;]"
  ["ee88bd", :undef], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+E23D -> "[&lt;&lt;]"
  ["ee8cb2", "ee9aa0"], # [HEAVY LARGE CIRCLE] U+E332 -> U+E6A0
  ["ee8cb3", :undef], # [CROSS MARK] U+E333 -> "[×]"
  ["ee80a1", "ee9c82"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E021 -> U+E702
  ["ee80a0", :undef], # [BLACK QUESTION MARK ORNAMENT] U+E020 -> "[？]"
  ["ee8cb6", :undef], # [WHITE QUESTION MARK ORNAMENT] U+E336 -> "[？]"
  ["ee8cb7", "ee9c82"], # [WHITE EXCLAMATION MARK ORNAMENT] U+E337 -> U+E702
  ["ee8891", "ee9b9f"], # [DOUBLE CURLY LOOP] U+E211 -> U+E6DF
  ["ee80a2", "ee9bac"], # [HEAVY BLACK HEART] U+E022 -> U+E6EC
  ["ee8ca7", "ee9bad"], # [BEATING HEART] U+E327 -> U+E6ED
  ["ee80a3", "ee9bae"], # [BROKEN HEART] U+E023 -> U+E6EE
  ["ee8ca8", "ee9bad"], # [GROWING HEART] U+E328 -> U+E6ED
  ["ee8ca9", "ee9bac"], # [HEART WITH ARROW] U+E329 -> U+E6EC
  ["ee8caa", "ee9bac"], # [BLUE HEART] U+E32A -> U+E6EC
  ["ee8cab", "ee9bac"], # [GREEN HEART] U+E32B -> U+E6EC
  ["ee8cac", "ee9bac"], # [YELLOW HEART] U+E32C -> U+E6EC
  ["ee8cad", "ee9bac"], # [PURPLE HEART] U+E32D -> U+E6EC
  ["ee90b7", "ee9bac"], # [HEART WITH RIBBON] U+E437 -> U+E6EC
  ["ee8884", "ee9bb8"], # [HEART DECORATION] U+E204 -> U+E6F8
  ["ee888c", "ee9a8d"], # [BLACK HEART SUIT] U+E20C -> U+E68D
  ["ee888e", "ee9a8e"], # [BLACK SPADE SUIT] U+E20E -> U+E68E
  ["ee888d", "ee9a8f"], # [BLACK DIAMOND SUIT] U+E20D -> U+E68F
  ["ee888f", "ee9a90"], # [BLACK CLUB SUIT] U+E20F -> U+E690
  ["ee8c8e", "ee99bf"], # [SMOKING SYMBOL] U+E30E -> U+E67F
  ["ee8888", "ee9a80"], # [NO SMOKING SYMBOL] U+E208 -> U+E680
  ["ee888a", "ee9a9b"], # [WHEELCHAIR SYMBOL] U+E20A -> U+E69B
  ["ee8992", "ee9cb7"], # [WARNING SIGN] U+E252 -> U+E737
  ["ee84b6", "ee9c9d"], # [BICYCLE] U+E136 -> U+E71D
  ["ee8881", "ee9cb3"], # [PEDESTRIAN] U+E201 -> U+E733
  ["ee84b8", :undef], # [MENS SYMBOL] U+E138 -> "[♂]"
  ["ee84b9", :undef], # [WOMENS SYMBOL] U+E139 -> "[♀]"
  ["ee84bf", "ee9bb7"], # [BATH] U+E13F -> U+E6F7
  ["ee8591", "ee99ae"], # [RESTROOM] U+E151 -> U+E66E
  ["ee8580", "ee99ae"], # [TOILET] U+E140 -> U+E66E
  ["ee8c89", "ee99ae"], # [WATER CLOSET] U+E309 -> U+E66E
  ["ee84ba", :undef], # [BABY SYMBOL] U+E13A -> "[赤ちゃん]"
  ["ee8894", :undef], # [SQUARED COOL] U+E214 -> "[COOL]"
  ["ee88a9", "ee9b98"], # [SQUARED ID] U+E229 -> U+E6D8
  ["ee8892", "ee9b9d"], # [SQUARED NEW] U+E212 -> U+E6DD
  ["ee898d", "ee9c8b"], # [SQUARED OK] U+E24D -> U+E70B
  ["ee8893", :undef], # [SQUARED UP WITH EXCLAMATION MARK] U+E213 -> "[UP!]"
  ["ee84ae", :undef], # [SQUARED VS] U+E12E -> "[VS]"
  ["ee8883", :undef], # [SQUARED KATAKANA KOKO] U+E203 -> "[ココ]"
  ["ee88a8", :undef], # [SQUARED KATAKANA SA] U+E228 -> "[サービス]"
  ["ee88ab", "ee9cb9"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+E22B -> U+E739
  ["ee88aa", "ee9cbb"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+E22A -> U+E73B
  ["ee8895", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6709] U+E215 -> "[有]"
  ["ee8896", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7121] U+E216 -> "[無]"
  ["ee8897", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6708] U+E217 -> "[月]"
  ["ee8898", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7533] U+E218 -> "[申]"
  ["ee88a7", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+E227 -> "[割]"
  ["ee88ac", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+E22C -> "[指]"
  ["ee88ad", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+E22D -> "[営]"
  ["ee8c95", "ee9cb4"], # [CIRCLED IDEOGRAPH SECRET] U+E315 -> U+E734
  ["ee8c8d", :undef], # [CIRCLED IDEOGRAPH CONGRATULATION] U+E30D -> "[祝]"
  ["ee88a6", :undef], # [CIRCLED IDEOGRAPH ADVANTAGE] U+E226 -> "[得]"
  ["ee848f", "ee9bbb"], # [ELECTRIC LIGHT BULB] U+E10F -> U+E6FB
  ["ee8cb4", "ee9bbc"], # [ANGER SYMBOL] U+E334 -> U+E6FC
  ["ee8c91", "ee9bbe"], # [BOMB] U+E311 -> U+E6FE
  ["ee84bc", "ee9c81"], # [SLEEPING SYMBOL] U+E13C -> U+E701
  ["ee8cb1", "ee9c86"], # [SPLASHING SWEAT SYMBOL] U+E331 -> U+E706
  ["ee8cb0", "ee9c88"], # [DASH SYMBOL] U+E330 -> U+E708
  ["ee819a", :undef], # [PILE OF POO] U+E05A -> "[ウンチ]"
  ["ee858c", :undef], # [FLEXED BICEPS] U+E14C -> "[力こぶ]"
  ["ee8cae", "ee9bba"], # [SPARKLES] U+E32E -> U+E6FA
  ["ee8885", "ee9bb8"], # [EIGHT POINTED BLACK STAR] U+E205 -> U+E6F8
  ["ee8886", "ee9bb8"], # [EIGHT SPOKED ASTERISK] U+E206 -> U+E6F8
  ["ee8899", "ee9a9c"], # [LARGE RED CIRCLE] U+E219 -> U+E69C
  ["ee889a", "ee9a9c"], # [BLACK SQUARE BUTTON] U+E21A -> U+E69C
  ["ee889b", "ee9a9c"], # [WHITE SQUARE BUTTON] U+E21B -> U+E69C
  ["ee8caf", :undef], # [WHITE MEDIUM STAR] U+E32F -> "[☆]"
  ["ee8581", :undef], # [SPEAKER WITH THREE SOUND WAVES] U+E141 -> "[スピーカ]"
  ["ee8494", "ee9b9c"], # [LEFT-POINTING MAGNIFYING GLASS] U+E114 -> U+E6DC
  ["ee8584", "ee9b99"], # [LOCK] U+E144 -> U+E6D9
  ["ee8585", "ee9b99"], # [OPEN LOCK] U+E145 -> U+E6D9
  ["ee80bf", "ee9b99"], # [KEY] U+E03F -> U+E6D9
  ["ee8ca5", "ee9c93"], # [BELL] U+E325 -> U+E713
  ["ee898c", :undef], # [TOP WITH UPWARDS ARROW ABOVE] U+E24C -> "[TOP]"
  ["ee8090", "ee9a93"], # [RAISED FIST] U+E010 -> U+E693
  ["ee8092", "ee9a95"], # [RAISED HAND] U+E012 -> U+E695
  ["ee8091", "ee9a94"], # [VICTORY HAND] U+E011 -> U+E694
  ["ee808d", "ee9bbd"], # [FISTED HAND SIGN] U+E00D -> U+E6FD
  ["ee808e", "ee9ca7"], # [THUMBS UP SIGN] U+E00E -> U+E727
  ["ee808f", :undef], # [WHITE UP POINTING INDEX] U+E00F -> "[人差し指]"
  ["ee88ae", :undef], # [WHITE UP POINTING BACKHAND INDEX] U+E22E -> "[↑]"
  ["ee88af", :undef], # [WHITE DOWN POINTING BACKHAND INDEX] U+E22F -> "[↓]"
  ["ee88b0", :undef], # [WHITE LEFT POINTING BACKHAND INDEX] U+E230 -> "[←]"
  ["ee88b1", :undef], # [WHITE RIGHT POINTING BACKHAND INDEX] U+E231 -> "[→]"
  ["ee909e", "ee9a95"], # [WAVING HAND SIGN] U+E41E -> U+E695
  ["ee909f", :undef], # [CLAPPING HANDS SIGN] U+E41F -> "[拍手]"
  ["ee90a0", "ee9c8b"], # [OK HAND SIGN] U+E420 -> U+E70B
  ["ee90a1", "ee9c80"], # [THUMBS DOWN SIGN] U+E421 -> U+E700
  ["ee90a2", "ee9a95"], # [OPEN HANDS SIGN] U+E422 -> U+E695
  ["ee94b8", :undef], # [EMOJI COMPATIBILITY SYMBOL-50] U+E538 -> U+3013 (GETA)
  ["ee94b9", :undef], # [EMOJI COMPATIBILITY SYMBOL-51] U+E539 -> U+3013 (GETA)
  ["ee94ba", :undef], # [EMOJI COMPATIBILITY SYMBOL-52] U+E53A -> U+3013 (GETA)
  ["ee94bb", :undef], # [EMOJI COMPATIBILITY SYMBOL-53] U+E53B -> U+3013 (GETA)
  ["ee94bc", :undef], # [EMOJI COMPATIBILITY SYMBOL-54] U+E53C -> "[v"
  ["ee94bd", :undef], # [EMOJI COMPATIBILITY SYMBOL-55] U+E53D -> "oda"
  ["ee94be", :undef], # [EMOJI COMPATIBILITY SYMBOL-56] U+E53E -> "fone]"
  ["ee8994", :undef], # [EMOJI COMPATIBILITY SYMBOL-57] U+E254 -> U+3013 (GETA)
  ["ee8995", :undef], # [EMOJI COMPATIBILITY SYMBOL-58] U+E255 -> U+3013 (GETA)
  ["ee8996", :undef], # [EMOJI COMPATIBILITY SYMBOL-59] U+E256 -> U+3013 (GETA)
  ["ee8997", :undef], # [EMOJI COMPATIBILITY SYMBOL-60] U+E257 -> U+3013 (GETA)
  ["ee8998", :undef], # [EMOJI COMPATIBILITY SYMBOL-61] U+E258 -> U+3013 (GETA)
  ["ee8999", :undef], # [EMOJI COMPATIBILITY SYMBOL-62] U+E259 -> U+3013 (GETA)
  ["ee899a", :undef], # [EMOJI COMPATIBILITY SYMBOL-63] U+E25A -> U+3013 (GETA)
  ["ee948a", :undef], # [EMOJI COMPATIBILITY SYMBOL-64] U+E50A -> U+3013 (GETA)
]

EMOJI_EXCHANGE_TBL['UTF8-SoftBank']['UTF8-KDDI'] = [
  ["ee818a", "ee9288"], # [BLACK SUN WITH RAYS] U+E04A -> U+E488
  ["ee8189", "ee928d"], # [CLOUD] U+E049 -> U+E48D
  ["ee818b", "ee928c"], # [UMBRELLA WITH RAIN DROPS] U+E04B -> U+E48C
  ["ee8188", "ee9285"], # [SNOWMAN WITHOUT SNOW] U+E048 -> U+E485
  ["ee84bd", "ee9287"], # [HIGH VOLTAGE SIGN] U+E13D -> U+E487
  ["ee9183", "ee91a9"], # [CYCLONE] U+E443 -> U+E469
  ["ee90bc", "eeaba8"], # [CLOSED UMBRELLA] U+E43C -> U+EAE8
  ["ee918b", "eeabb1"], # [NIGHT WITH STARS] U+E44B -> U+EAF1
  ["ee818d", "eeabb4"], # [SUNRISE OVER MOUNTAINS] U+E04D -> U+EAF4
  ["ee9189", "eeabb4"], # [SUNRISE] U+E449 -> U+EAF4
  ["ee8586", "ee979a"], # [CITYSCAPE AT DUSK] U+E146 -> U+E5DA
  ["ee918a", "ee979a"], # [SUNSET OVER BUILDINGS] U+E44A -> U+E5DA
  ["ee918c", "eeabb2"], # [RAINBOW] U+E44C -> U+EAF2
  ["ee90be", "eeadbc"], # [WATER WAVE] U+E43E -> U+EB7C
  ["ee818c", "ee9286"], # [CRESCENT MOON] U+E04C -> U+E486
  ["ee8cb5", "ee928b"], # [GLOWING STAR] U+E335 -> U+E48B
  ["ee80a4", "ee9694"], # [CLOCK FACE ONE OCLOCK] U+E024 -> U+E594
  ["ee80a5", "ee9694"], # [CLOCK FACE TWO OCLOCK] U+E025 -> U+E594
  ["ee80a6", "ee9694"], # [CLOCK FACE THREE OCLOCK] U+E026 -> U+E594
  ["ee80a7", "ee9694"], # [CLOCK FACE FOUR OCLOCK] U+E027 -> U+E594
  ["ee80a8", "ee9694"], # [CLOCK FACE FIVE OCLOCK] U+E028 -> U+E594
  ["ee80a9", "ee9694"], # [CLOCK FACE SIX OCLOCK] U+E029 -> U+E594
  ["ee80aa", "ee9694"], # [CLOCK FACE SEVEN OCLOCK] U+E02A -> U+E594
  ["ee80ab", "ee9694"], # [CLOCK FACE EIGHT OCLOCK] U+E02B -> U+E594
  ["ee80ac", "ee9694"], # [CLOCK FACE NINE OCLOCK] U+E02C -> U+E594
  ["ee80ad", "ee9694"], # [CLOCK FACE TEN OCLOCK] U+E02D -> U+E594
  ["ee80ae", "ee9694"], # [CLOCK FACE ELEVEN OCLOCK] U+E02E -> U+E594
  ["ee80af", "ee9694"], # [CLOCK FACE TWELVE OCLOCK] U+E02F -> U+E594
  ["ee88bf", "ee928f"], # [ARIES] U+E23F -> U+E48F
  ["ee8980", "ee9290"], # [TAURUS] U+E240 -> U+E490
  ["ee8981", "ee9291"], # [GEMINI] U+E241 -> U+E491
  ["ee8982", "ee9292"], # [CANCER] U+E242 -> U+E492
  ["ee8983", "ee9293"], # [LEO] U+E243 -> U+E493
  ["ee8984", "ee9294"], # [VIRGO] U+E244 -> U+E494
  ["ee8985", "ee9295"], # [LIBRA] U+E245 -> U+E495
  ["ee8986", "ee9296"], # [SCORPIUS] U+E246 -> U+E496
  ["ee8987", "ee9297"], # [SAGITTARIUS] U+E247 -> U+E497
  ["ee8988", "ee9298"], # [CAPRICORN] U+E248 -> U+E498
  ["ee8989", "ee9299"], # [AQUARIUS] U+E249 -> U+E499
  ["ee898a", "ee929a"], # [PISCES] U+E24A -> U+E49A
  ["ee898b", "ee929b"], # [OPHIUCHUS] U+E24B -> U+E49B
  ["ee8490", "ee9493"], # [FOUR LEAF CLOVER] U+E110 -> U+E513
  ["ee8c84", "ee93a4"], # [TULIP] U+E304 -> U+E4E4
  ["ee8498", "ee938e"], # [MAPLE LEAF] U+E118 -> U+E4CE
  ["ee80b0", "ee938a"], # [CHERRY BLOSSOM] U+E030 -> U+E4CA
  ["ee80b2", "ee96ba"], # [ROSE] U+E032 -> U+E5BA
  ["ee8499", "ee978d"], # [FALLEN LEAF] U+E119 -> U+E5CD
  ["ee9187", "ee978d"], # [LEAF FLUTTERING IN WIND] U+E447 -> U+E5CD
  ["ee8c83", "eeaa94"], # [HIBISCUS] U+E303 -> U+EA94
  ["ee8c85", "ee93a3"], # [SUNFLOWER] U+E305 -> U+E4E3
  ["ee8c87", "ee93a2"], # [PALM TREE] U+E307 -> U+E4E2
  ["ee8c88", "eeaa96"], # [CACTUS] U+E308 -> U+EA96
  ["ee9184", :undef], # [EAR OF RICE] U+E444 -> "[稲穂]"
  ["ee8d85", "eeaab9"], # [RED APPLE] U+E345 -> U+EAB9
  ["ee8d86", "eeaaba"], # [TANGERINE] U+E346 -> U+EABA
  ["ee8d87", "ee9394"], # [STRAWBERRY] U+E347 -> U+E4D4
  ["ee8d88", "ee938d"], # [WATERMELON] U+E348 -> U+E4CD
  ["ee8d89", "eeaabb"], # [TOMATO] U+E349 -> U+EABB
  ["ee8d8a", "eeaabc"], # [AUBERGINE] U+E34A -> U+EABC
  ["ee9099", "ee96a4"], # [EYES] U+E419 -> U+E5A4
  ["ee909b", "ee96a5"], # [EAR] U+E41B -> U+E5A5
  ["ee909a", "eeab90"], # [NOSE] U+E41A -> U+EAD0
  ["ee909c", "eeab91"], # [MOUTH] U+E41C -> U+EAD1
  ["ee8c9c", "ee9489"], # [LIPSTICK] U+E31C -> U+E509
  ["ee8c9d", "eeaaa0"], # [NAIL POLISH] U+E31D -> U+EAA0
  ["ee8c9e", "ee948b"], # [FACE MASSAGE] U+E31E -> U+E50B
  ["ee8c9f", "eeaaa1"], # [HAIRCUT] U+E31F -> U+EAA1
  ["ee8ca0", "eeaaa2"], # [BARBER POLE] U+E320 -> U+EAA2
  ["ee8081", "ee93bc"], # [BOY] U+E001 -> U+E4FC
  ["ee8082", "ee93ba"], # [GIRL] U+E002 -> U+E4FA
  ["ee8084", "ee93bc"], # [MAN] U+E004 -> U+E4FC
  ["ee8085", "ee93ba"], # [WOMAN] U+E005 -> U+E4FA
  ["ee90a8", :undef], # [MAN AND WOMAN HOLDING HANDS] U+E428 -> "[カップル]"
  ["ee8592", "ee979d"], # [POLICE OFFICER] U+E152 -> U+E5DD
  ["ee90a9", "eeab9b"], # [WOMAN WITH BUNNY EARS] U+E429 -> U+EADB
  ["ee9495", "eeac93"], # [WESTERN PERSON] U+E515 -> U+EB13
  ["ee9496", "eeac94"], # [MAN WITH GUA PI MAO] U+E516 -> U+EB14
  ["ee9497", "eeac95"], # [MAN WITH TURBAN] U+E517 -> U+EB15
  ["ee9498", "eeac96"], # [OLDER MAN] U+E518 -> U+EB16
  ["ee9499", "eeac97"], # [OLDER WOMAN] U+E519 -> U+EB17
  ["ee949a", "eeac98"], # [BABY] U+E51A -> U+EB18
  ["ee949b", "eeac99"], # [CONSTRUCTION WORKER] U+E51B -> U+EB19
  ["ee949c", "eeac9a"], # [PRINCESS] U+E51C -> U+EB1A
  ["ee849b", "ee938b"], # [GHOST] U+E11B -> U+E4CB
  ["ee818e", "ee96bf"], # [BABY ANGEL] U+E04E -> U+E5BF
  ["ee848c", "ee948e"], # [EXTRATERRESTRIAL ALIEN] U+E10C -> U+E50E
  ["ee84ab", "ee93ac"], # [ALIEN MONSTER] U+E12B -> U+E4EC
  ["ee849a", "ee93af"], # [IMP] U+E11A -> U+E4EF
  ["ee849c", "ee93b8"], # [SKULL] U+E11C -> U+E4F8
  ["ee8993", :undef], # [INFORMATION DESK PERSON] U+E253 -> "[案内]"
  ["ee949e", :undef], # [GUARDSMAN] U+E51E -> "[衛兵]"
  ["ee949f", "eeac9c"], # [DANCER] U+E51F -> U+EB1C
  ["ee94ad", "eeaca2"], # [SNAKE] U+E52D -> U+EB22
  ["ee84b4", "ee9398"], # [HORSE] U+E134 -> U+E4D8
  ["ee94ae", "eeaca3"], # [CHICKEN] U+E52E -> U+EB23
  ["ee94af", "eeaca4"], # [BOAR] U+E52F -> U+EB24
  ["ee94b0", "eeaca5"], # [BACTRIAN CAMEL] U+E530 -> U+EB25
  ["ee94a6", "eeac9f"], # [ELEPHANT] U+E526 -> U+EB1F
  ["ee94a7", "eeaca0"], # [KOALA] U+E527 -> U+EB20
  ["ee94a8", "ee9399"], # [MONKEY] U+E528 -> U+E4D9
  ["ee94a9", "ee928f"], # [SHEEP] U+E529 -> U+E48F
  ["ee848a", "ee9787"], # [OCTOPUS] U+E10A -> U+E5C7
  ["ee9181", "eeabac"], # [SPIRAL SHELL] U+E441 -> U+EAEC
  ["ee94a5", "eeac9e"], # [BUG] U+E525 -> U+EB1E
  ["ee94a2", "eeac9d"], # [TROPICAL FISH] U+E522 -> U+EB1D
  ["ee94a3", "ee93a0"], # [BABY CHICK] U+E523 -> U+E4E0
  ["ee94a1", "ee93a0"], # [BIRD] U+E521 -> U+E4E0
  ["ee8195", "ee939c"], # [PENGUIN] U+E055 -> U+E4DC
  ["ee8099", "ee929a"], # [FISH] U+E019 -> U+E49A
  ["ee94a0", "eeac9b"], # [DOLPHIN] U+E520 -> U+EB1B
  ["ee8193", "ee9782"], # [MOUSE FACE] U+E053 -> U+E5C2
  ["ee8190", "ee9780"], # [TIGER FACE] U+E050 -> U+E5C0
  ["ee818f", "ee939b"], # [CAT FACE] U+E04F -> U+E4DB
  ["ee8194", "ee91b0"], # [SPOUTING WHALE] U+E054 -> U+E470
  ["ee809a", "ee9398"], # [HORSE FACE] U+E01A -> U+E4D8
  ["ee8489", "ee9399"], # [MONKEY FACE] U+E109 -> U+E4D9
  ["ee8192", "ee93a1"], # [DOG FACE] U+E052 -> U+E4E1
  ["ee848b", "ee939e"], # [PIG FACE] U+E10B -> U+E4DE
  ["ee8191", "ee9781"], # [BEAR FACE] U+E051 -> U+E5C1
  ["ee94a4", :undef], # [HAMSTER FACE] U+E524 -> "[ハムスター]"
  ["ee94aa", "ee93a1"], # [WOLF FACE] U+E52A -> U+E4E1
  ["ee94ab", "eeaca1"], # [COW FACE] U+E52B -> U+EB21
  ["ee94ac", "ee9397"], # [RABBIT FACE] U+E52C -> U+E4D7
  ["ee94b1", "ee939a"], # [FROG FACE] U+E531 -> U+E4DA
  ["ee8199", "ee91b2"], # [ANGRY FACE] U+E059 -> U+E472
  ["ee9090", "eeab8a"], # [ASTONISHED FACE] U+E410 -> U+EACA
  ["ee8198", "eeab80"], # [DISAPPOINTED FACE] U+E058 -> U+EAC0
  ["ee908f", "eeab8b"], # [EXASPERATED FACE] U+E40F -> U+EACB
  ["ee908e", "eeab89"], # [EXPRESSIONLESS FACE] U+E40E -> U+EAC9
  ["ee8486", "ee9784"], # [FACE WITH HEART-SHAPED EYES] U+E106 -> U+E5C4
  ["ee8485", "ee93a7"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E105 -> U+E4E7
  ["ee9089", "ee93a7"], # [FACE WITH STUCK-OUT TONGUE] U+E409 -> U+E4E7
  ["ee9098", "eeab8f"], # [FACE THROWING A KISS] U+E418 -> U+EACF
  ["ee9097", "eeab8e"], # [FACE KISSING] U+E417 -> U+EACE
  ["ee908c", "eeab87"], # [FACE WITH MASK] U+E40C -> U+EAC7
  ["ee908d", "eeab88"], # [FLUSHED FACE] U+E40D -> U+EAC8
  ["ee8197", "ee91b1"], # [HAPPY FACE WITH OPEN MOUTH] U+E057 -> U+E471
  ["ee9084", "eeae80"], # [HAPPY FACE WITH GRIN] U+E404 -> U+EB80
  ["ee9092", "eeada4"], # [HAPPY AND CRYING FACE] U+E412 -> U+EB64
  ["ee8196", "eeab8d"], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+E056 -> U+EACD
  ["ee9094", "ee93bb"], # [WHITE SMILING FACE] U+E414 -> U+E4FB
  ["ee9095", "ee91b1"], # [HAPPY FACE WITH OPEN MOUTH AND RAISED EYEBROWS] U+E415 -> U+E471
  ["ee9093", "eeada9"], # [CRYING FACE] U+E413 -> U+EB69
  ["ee9091", "ee91b3"], # [LOUDLY CRYING FACE] U+E411 -> U+E473
  ["ee908b", "eeab86"], # [FEARFUL FACE] U+E40B -> U+EAC6
  ["ee9086", "eeab82"], # [PERSEVERING FACE] U+E406 -> U+EAC2
  ["ee9096", "eead9d"], # [POUTING FACE] U+E416 -> U+EB5D
  ["ee908a", "eeab85"], # [RELIEVED FACE] U+E40A -> U+EAC5
  ["ee9087", "eeab83"], # [CONFOUNDED FACE] U+E407 -> U+EAC3
  ["ee9083", "eeab80"], # [PENSIVE FACE] U+E403 -> U+EAC0
  ["ee8487", "ee9785"], # [FACE SCREAMING IN FEAR] U+E107 -> U+E5C5
  ["ee9088", "eeab84"], # [SLEEPY FACE] U+E408 -> U+EAC4
  ["ee9082", "eeaabf"], # [SMIRKING FACE] U+E402 -> U+EABF
  ["ee8488", "ee9786"], # [FACE WITH COLD SWEAT] U+E108 -> U+E5C6
  ["ee9081", "ee9786"], # [DISAPPOINTED BUT RELIEVED FACE] U+E401 -> U+E5C6
  ["ee9085", "ee9783"], # [WINKING FACE] U+E405 -> U+E5C3
  ["ee90a3", "eeab97"], # [FACE WITH NO GOOD GESTURE] U+E423 -> U+EAD7
  ["ee90a4", "eeab98"], # [FACE WITH OK GESTURE] U+E424 -> U+EAD8
  ["ee90a6", "eeab99"], # [PERSON BOWING DEEPLY] U+E426 -> U+EAD9
  ["ee90a7", "eeae86"], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+E427 -> U+EB86
  ["ee909d", "eeab92"], # [PERSON WITH FOLDED HANDS] U+E41D -> U+EAD2
  ["ee80b6", "ee92ab"], # [HOUSE BUILDING] U+E036 -> U+E4AB
  ["ee80b8", "ee92ad"], # [OFFICE BUILDING] U+E038 -> U+E4AD
  ["ee8593", "ee979e"], # [JAPANESE POST OFFICE] U+E153 -> U+E5DE
  ["ee8595", "ee979f"], # [HOSPITAL] U+E155 -> U+E5DF
  ["ee858d", "ee92aa"], # [BANK] U+E14D -> U+E4AA
  ["ee8594", "ee92a3"], # [AUTOMATED TELLER MACHINE] U+E154 -> U+E4A3
  ["ee8598", "eeaa81"], # [HOTEL] U+E158 -> U+EA81
  ["ee9481", "eeabb3"], # [LOVE HOTEL] U+E501 -> U+EAF3
  ["ee8596", "ee92a4"], # [CONVENIENCE STORE] U+E156 -> U+E4A4
  ["ee8597", "eeaa80"], # [SCHOOL] U+E157 -> U+EA80
  ["ee80b7", "ee96bb"], # [CHURCH] U+E037 -> U+E5BB
  ["ee84a1", "ee978f"], # [FOUNTAIN] U+E121 -> U+E5CF
  ["ee9484", "eeabb6"], # [DEPARTMENT STORE] U+E504 -> U+EAF6
  ["ee9485", "eeabb7"], # [JAPANESE CASTLE] U+E505 -> U+EAF7
  ["ee9486", "eeabb8"], # [EUROPEAN CASTLE] U+E506 -> U+EAF8
  ["ee9488", "eeabb9"], # [FACTORY] U+E508 -> U+EAF9
  ["ee80bb", "ee96bd"], # [MOUNT FUJI] U+E03B -> U+E5BD
  ["ee9489", "ee9380"], # [TOKYO TOWER] U+E509 -> U+E4C0
  ["ee949d", :undef], # [STATUE OF LIBERTY] U+E51D -> "[自由の女神]"
  ["ee8087", "eeacab"], # [ATHLETIC SHOE] U+E007 -> U+EB2B
  ["ee84be", "ee949a"], # [HIGH-HEELED SHOE] U+E13E -> U+E51A
  ["ee8c9a", "ee949a"], # [WOMANS SANDAL] U+E31A -> U+E51A
  ["ee8c9b", "eeaa9f"], # [WOMANS BOOTS] U+E31B -> U+EA9F
  ["ee94b6", "eeacaa"], # [FOOTPRINTS] U+E536 -> U+EB2A
  ["ee8086", "ee96b6"], # [T-SHIRT] U+E006 -> U+E5B6
  ["ee848e", "ee9789"], # [CROWN] U+E10E -> U+E5C9
  ["ee8c82", "eeaa93"], # [NECKTIE] U+E302 -> U+EA93
  ["ee8c98", "eeaa9e"], # [WOMANS HAT] U+E318 -> U+EA9E
  ["ee8c99", "eeadab"], # [DRESS] U+E319 -> U+EB6B
  ["ee8ca1", "eeaaa3"], # [KIMONO] U+E321 -> U+EAA3
  ["ee8ca2", "eeaaa4"], # [BIKINI] U+E322 -> U+EAA4
  ["ee8ca3", "ee929c"], # [HANDBAG] U+E323 -> U+E49C
  ["ee84af", "ee9387"], # [MONEY BAG] U+E12F -> U+E4C7
  ["ee8589", :undef], # [CURRENCY EXCHANGE] U+E149 -> "[$￥]"
  ["ee858a", "ee979c"], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+E14A -> U+E5DC
  ["ee9493", "eeac91"], # [REGIONAL INDICATOR SYMBOL LETTERS CN] U+E513 -> U+EB11
  ["ee948e", "eeac8e"], # [REGIONAL INDICATOR SYMBOL LETTERS DE] U+E50E -> U+EB0E
  ["ee9491", "ee9795"], # [REGIONAL INDICATOR SYMBOL LETTERS ES] U+E511 -> U+E5D5
  ["ee948d", "eeabba"], # [REGIONAL INDICATOR SYMBOL LETTERS FR] U+E50D -> U+EAFA
  ["ee9490", "eeac90"], # [REGIONAL INDICATOR SYMBOL LETTERS GB] U+E510 -> U+EB10
  ["ee948f", "eeac8f"], # [REGIONAL INDICATOR SYMBOL LETTERS IT] U+E50F -> U+EB0F
  ["ee948b", "ee938c"], # [REGIONAL INDICATOR SYMBOL LETTERS JP] U+E50B -> U+E4CC
  ["ee9494", "eeac92"], # [REGIONAL INDICATOR SYMBOL LETTERS KR] U+E514 -> U+EB12
  ["ee9492", "ee9796"], # [REGIONAL INDICATOR SYMBOL LETTERS RU] U+E512 -> U+E5D6
  ["ee948c", "ee95b3"], # [REGIONAL INDICATOR SYMBOL LETTERS US] U+E50C -> U+E573
  ["ee849d", "ee91bb"], # [FIRE] U+E11D -> U+E47B
  ["ee8496", "ee978b"], # [HAMMER] U+E116 -> U+E5CB
  ["ee8493", "ee948a"], # [PISTOL] U+E113 -> U+E50A
  ["ee88be", "eeaa8f"], # [SIX POINTED STAR WITH MIDDLE DOT] U+E23E -> U+EA8F
  ["ee8889", "ee9280"], # [JAPANESE SYMBOL FOR BEGINNER] U+E209 -> U+E480
  ["ee80b1", "ee9789"], # [TRIDENT EMBLEM] U+E031 -> U+E5C9
  ["ee84bb", "ee9490"], # [SYRINGE] U+E13B -> U+E510
  ["ee8c8f", "eeaa9a"], # [PILL] U+E30F -> U+EA9A
  ["ee94b2", "eeaca6"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+E532 -> U+EB26
  ["ee94b3", "eeaca7"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+E533 -> U+EB27
  ["ee94b4", "eeaca9"], # [NEGATIVE SQUARED AB] U+E534 -> U+EB29
  ["ee94b5", "eeaca8"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+E535 -> U+EB28
  ["ee8c94", "ee969f"], # [RIBBON] U+E314 -> U+E59F
  ["ee8492", "ee938f"], # [WRAPPED PRESENT] U+E112 -> U+E4CF
  ["ee8d8b", "ee96a0"], # [BIRTHDAY CAKE] U+E34B -> U+E5A0
  ["ee80b3", "ee9389"], # [CHRISTMAS TREE] U+E033 -> U+E4C9
  ["ee9188", "eeabb0"], # [FATHER CHRISTMAS] U+E448 -> U+EAF0
  ["ee8583", "ee9799"], # [CROSSED FLAGS] U+E143 -> U+E5D9
  ["ee8497", "ee978c"], # [FIREWORKS] U+E117 -> U+E5CC
  ["ee8c90", "eeaa9b"], # [BALLOON] U+E310 -> U+EA9B
  ["ee8c92", "eeaa9c"], # [PARTY POPPER] U+E312 -> U+EA9C
  ["ee90b6", "eeaba3"], # [PINE DECORATION] U+E436 -> U+EAE3
  ["ee90b8", "eeaba4"], # [JAPANESE DOLLS] U+E438 -> U+EAE4
  ["ee90b9", "eeaba5"], # [GRADUATION CAP] U+E439 -> U+EAE5
  ["ee90ba", "eeaba6"], # [SCHOOL SATCHEL] U+E43A -> U+EAE6
  ["ee90bb", "eeaba7"], # [CARP STREAMER] U+E43B -> U+EAE7
  ["ee9180", "eeabab"], # [FIREWORK SPARKLER] U+E440 -> U+EAEB
  ["ee9182", "eeabad"], # [WIND CHIME] U+E442 -> U+EAED
  ["ee9185", "eeabae"], # [JACK-O-LANTERN] U+E445 -> U+EAEE
  ["ee9186", "eeabaf"], # [MOON VIEWING CEREMONY] U+E446 -> U+EAEF
  ["ee8089", "ee9696"], # [BLACK TELEPHONE] U+E009 -> U+E596
  ["ee808a", "ee9688"], # [MOBILE PHONE] U+E00A -> U+E588
  ["ee8484", "eeac88"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+E104 -> U+EB08
  ["ee8c81", "eeaa92"], # [MEMO] U+E301 -> U+EA92
  ["ee808b", "ee94a0"], # [FAX MACHINE] U+E00B -> U+E520
  ["ee8483", "eeada2"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+E103 -> U+EB62
  ["ee8481", "eeac8a"], # [CLOSED MAILBOX WITH RAISED FLAG] U+E101 -> U+EB0A
  ["ee8482", "ee949b"], # [POSTBOX] U+E102 -> U+E51B
  ["ee8582", "ee9491"], # [PUBLIC ADDRESS LOUDSPEAKER] U+E142 -> U+E511
  ["ee8c97", "ee9491"], # [CHEERING MEGAPHONE] U+E317 -> U+E511
  ["ee858b", "ee92a8"], # [SATELLITE ANTENNA] U+E14B -> U+E4A8
  ["ee849f", :undef], # [SEAT] U+E11F -> "[いす]"
  ["ee808c", "ee96b8"], # [PERSONAL COMPUTER] U+E00C -> U+E5B8
  ["ee849e", "ee978e"], # [BRIEFCASE] U+E11E -> U+E5CE
  ["ee8c96", "ee9682"], # [MINIDISC] U+E316 -> U+E582
  ["ee84a6", "ee948c"], # [OPTICAL DISC] U+E126 -> U+E50C
  ["ee84a7", "ee948c"], # [DVD] U+E127 -> U+E50C
  ["ee8c93", "ee9496"], # [BLACK SCISSORS] U+E313 -> U+E516
  ["ee8588", "ee929f"], # [OPEN BOOK] U+E148 -> U+E49F
  ["ee8096", "ee92ba"], # [BASEBALL] U+E016 -> U+E4BA
  ["ee8094", "ee9699"], # [FLAG IN HOLE] U+E014 -> U+E599
  ["ee8095", "ee92b7"], # [TENNIS RACQUET AND BALL] U+E015 -> U+E4B7
  ["ee8098", "ee92b6"], # [SOCCER BALL] U+E018 -> U+E4B6
  ["ee8093", "eeaaac"], # [SKI AND SKI BOOT] U+E013 -> U+EAAC
  ["ee90aa", "ee969a"], # [BASKETBALL AND HOOP] U+E42A -> U+E59A
  ["ee84b2", "ee92b9"], # [CHEQUERED FLAG] U+E132 -> U+E4B9
  ["ee8495", "ee91ab"], # [RUNNER] U+E115 -> U+E46B
  ["ee8097", "eead81"], # [SURFER] U+E017 -> U+EB41
  ["ee84b1", "ee9793"], # [TROPHY] U+E131 -> U+E5D3
  ["ee90ab", "ee92bb"], # [AMERICAN FOOTBALL] U+E42B -> U+E4BB
  ["ee90ad", "eeab9e"], # [SWIMMER] U+E42D -> U+EADE
  ["ee809e", "ee92b5"], # [TRAIN] U+E01E -> U+E4B5
  ["ee90b4", "ee96bc"], # [METRO] U+E434 -> U+E5BC
  ["ee90b5", "ee92b0"], # [HIGH-SPEED TRAIN] U+E435 -> U+E4B0
  ["ee809f", "ee92b0"], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+E01F -> U+E4B0
  ["ee809b", "ee92b1"], # [AUTOMOBILE] U+E01B -> U+E4B1
  ["ee90ae", "ee92b1"], # [RECREATIONAL VEHICLE] U+E42E -> U+E4B1
  ["ee8599", "ee92af"], # [ONCOMING BUS] U+E159 -> U+E4AF
  ["ee8590", "ee92a7"], # [BUS STOP] U+E150 -> U+E4A7
  ["ee8882", "eeaa82"], # [SHIP] U+E202 -> U+EA82
  ["ee809d", "ee92b3"], # [AIRPLANE] U+E01D -> U+E4B3
  ["ee809c", "ee92b4"], # [SAILBOAT] U+E01C -> U+E4B4
  ["ee80b9", "eeadad"], # [STATION] U+E039 -> U+EB6D
  ["ee848d", "ee9788"], # [ROCKET] U+E10D -> U+E5C8
  ["ee84b5", "ee92b4"], # [SPEEDBOAT] U+E135 -> U+E4B4
  ["ee859a", "ee92b1"], # [TAXI] U+E15A -> U+E4B1
  ["ee90af", "ee92b2"], # [DELIVERY TRUCK] U+E42F -> U+E4B2
  ["ee90b0", "eeab9f"], # [FIRE ENGINE] U+E430 -> U+EADF
  ["ee90b1", "eeaba0"], # [AMBULANCE] U+E431 -> U+EAE0
  ["ee90b2", "eeaba1"], # [POLICE CAR] U+E432 -> U+EAE1
  ["ee80ba", "ee95b1"], # [FUEL PUMP] U+E03A -> U+E571
  ["ee858f", "ee92a6"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E14F -> U+E4A6
  ["ee858e", "ee91aa"], # [HORIZONTAL TRAFFIC LIGHT] U+E14E -> U+E46A
  ["ee84b7", "ee9797"], # [CONSTRUCTION SIGN] U+E137 -> U+E5D7
  ["ee84a3", "ee92bc"], # [HOT SPRINGS] U+E123 -> U+E4BC
  ["ee84a2", "ee9790"], # [TENT] U+E122 -> U+E5D0
  ["ee84a4", "ee91ad"], # [FERRIS WHEEL] U+E124 -> U+E46D
  ["ee90b3", "eeaba2"], # [ROLLER COASTER] U+E433 -> U+EAE2
  ["ee80bc", "ee9483"], # [MICROPHONE] U+E03C -> U+E503
  ["ee80bd", "ee9497"], # [MOVIE CAMERA] U+E03D -> U+E517
  ["ee9487", "ee9497"], # [CINEMA] U+E507 -> U+E517
  ["ee8c8a", "ee9488"], # [HEADPHONE] U+E30A -> U+E508
  ["ee9482", "ee969c"], # [ARTIST PALETTE] U+E502 -> U+E59C
  ["ee9483", "eeabb5"], # [TOP HAT] U+E503 -> U+EAF5
  ["ee84a5", "ee929e"], # [TICKET] U+E125 -> U+E49E
  ["ee8ca4", "ee92be"], # [CLAPPER BOARD] U+E324 -> U+E4BE
  ["ee84ad", "ee9791"], # [MAHJONG TILE RED DRAGON] U+E12D -> U+E5D1
  ["ee84b0", "ee9385"], # [DIRECT HIT] U+E130 -> U+E4C5
  ["ee84b3", "ee91ae"], # [SLOT MACHINE] U+E133 -> U+E46E
  ["ee90ac", "eeab9d"], # [BILLIARDS] U+E42C -> U+EADD
  ["ee80be", "ee96be"], # [MUSICAL NOTE] U+E03E -> U+E5BE
  ["ee8ca6", "ee9485"], # [MULTIPLE MUSICAL NOTES] U+E326 -> U+E505
  ["ee8180", :undef], # [SAXOPHONE] U+E040 -> "[サックス]"
  ["ee8181", "ee9486"], # [GUITAR] U+E041 -> U+E506
  ["ee8182", "eeab9c"], # [TRUMPET] U+E042 -> U+EADC
  ["ee84ac", :undef], # [PART ALTERNATION MARK] U+E12C -> "[歌記号]"
  ["ee8088", "ee9495"], # [CAMERA] U+E008 -> U+E515
  ["ee84aa", "ee9482"], # [TELEVISION] U+E12A -> U+E502
  ["ee84a8", "ee96b9"], # [RADIO] U+E128 -> U+E5B9
  ["ee84a9", "ee9680"], # [VIDEOCASSETTE] U+E129 -> U+E580
  ["ee8083", "ee93ab"], # [KISS MARK] U+E003 -> U+E4EB
  ["ee80b4", "ee9494"], # [RING] U+E034 -> U+E514
  ["ee80b5", "ee9494"], # [GEM STONE] U+E035 -> U+E514
  ["ee8491", "ee978a"], # [KISS] U+E111 -> U+E5CA
  ["ee8c86", "eeaa95"], # [BOUQUET] U+E306 -> U+EA95
  ["ee90a5", "eeab9a"], # [COUPLE WITH HEART] U+E425 -> U+EADA
  ["ee90bd", "ee96bb"], # [WEDDING] U+E43D -> U+E5BB
  ["ee8887", "eeaa83"], # [NO ONE UNDER EIGHTEEN SYMBOL] U+E207 -> U+EA83
  ["ee898e", "ee9598"], # [COPYRIGHT SIGN] U+E24E -> U+E558
  ["ee898f", "ee9599"], # [REGISTERED SIGN] U+E24F -> U+E559
  ["ee94b7", "ee958e"], # [TRADE MARK SIGN] U+E537 -> U+E54E
  ["ee8890", "eeae84"], # [HASH KEY] U+E210 -> U+EB84
  ["ee889c", "ee94a2"], # [KEYCAP 1] U+E21C -> U+E522
  ["ee889d", "ee94a3"], # [KEYCAP 2] U+E21D -> U+E523
  ["ee889e", "ee94a4"], # [KEYCAP 3] U+E21E -> U+E524
  ["ee889f", "ee94a5"], # [KEYCAP 4] U+E21F -> U+E525
  ["ee88a0", "ee94a6"], # [KEYCAP 5] U+E220 -> U+E526
  ["ee88a1", "ee94a7"], # [KEYCAP 6] U+E221 -> U+E527
  ["ee88a2", "ee94a8"], # [KEYCAP 7] U+E222 -> U+E528
  ["ee88a3", "ee94a9"], # [KEYCAP 8] U+E223 -> U+E529
  ["ee88a4", "ee94aa"], # [KEYCAP 9] U+E224 -> U+E52A
  ["ee88a5", "ee96ac"], # [KEYCAP 0] U+E225 -> U+E5AC
  ["ee888b", "eeaa84"], # [ANTENNA WITH BARS] U+E20B -> U+EA84
  ["ee8990", "eeaa90"], # [VIBRATION MODE] U+E250 -> U+EA90
  ["ee8991", "eeaa91"], # [MOBILE PHONE OFF] U+E251 -> U+EA91
  ["ee84a0", "ee9396"], # [HAMBURGER] U+E120 -> U+E4D6
  ["ee8d82", "ee9395"], # [RICE BALL] U+E342 -> U+E4D5
  ["ee8186", "ee9390"], # [SHORTCAKE] U+E046 -> U+E4D0
  ["ee8d80", "ee96b4"], # [STEAMING BOWL] U+E340 -> U+E5B4
  ["ee8cb9", "eeaaaf"], # [BREAD] U+E339 -> U+EAAF
  ["ee8587", "ee9391"], # [COOKING] U+E147 -> U+E4D1
  ["ee8cba", "eeaab0"], # [SOFT ICE CREAM] U+E33A -> U+EAB0
  ["ee8cbb", "eeaab1"], # [FRENCH FRIES] U+E33B -> U+EAB1
  ["ee8cbc", "eeaab2"], # [DANGO] U+E33C -> U+EAB2
  ["ee8cbd", "eeaab3"], # [RICE CRACKER] U+E33D -> U+EAB3
  ["ee8cbe", "eeaab4"], # [COOKED RICE] U+E33E -> U+EAB4
  ["ee8cbf", "eeaab5"], # [SPAGHETTI] U+E33F -> U+EAB5
  ["ee8d81", "eeaab6"], # [CURRY AND RICE] U+E341 -> U+EAB6
  ["ee8d83", "eeaab7"], # [ODEN] U+E343 -> U+EAB7
  ["ee8d84", "eeaab8"], # [SUSHI] U+E344 -> U+EAB8
  ["ee8d8c", "eeaabd"], # [BENTO BOX] U+E34C -> U+EABD
  ["ee8d8d", "eeaabe"], # [POT OF FOOD] U+E34D -> U+EABE
  ["ee90bf", "eeabaa"], # [SHAVED ICE] U+E43F -> U+EAEA
  ["ee8183", "ee92ac"], # [FORK AND KNIFE] U+E043 -> U+E4AC
  ["ee8185", "ee9697"], # [HOT BEVERAGE] U+E045 -> U+E597
  ["ee8184", "ee9382"], # [COCKTAIL GLASS] U+E044 -> U+E4C2
  ["ee8187", "ee9383"], # [BEER MUG] U+E047 -> U+E4C3
  ["ee8cb8", "eeaaae"], # [TEACUP WITHOUT HANDLE] U+E338 -> U+EAAE
  ["ee8c8b", "eeaa97"], # [SAKE BOTTLE AND CUP] U+E30B -> U+EA97
  ["ee8c8c", "eeaa98"], # [CLINKING BEER MUGS] U+E30C -> U+EA98
  ["ee88b6", "ee9595"], # [NORTH EAST ARROW] U+E236 -> U+E555
  ["ee88b8", "ee958d"], # [SOUTH EAST ARROW] U+E238 -> U+E54D
  ["ee88b7", "ee958c"], # [NORTH WEST ARROW] U+E237 -> U+E54C
  ["ee88b9", "ee9596"], # [SOUTH WEST ARROW] U+E239 -> U+E556
  ["ee88b2", "ee94bf"], # [UPWARDS BLACK ARROW] U+E232 -> U+E53F
  ["ee88b3", "ee9580"], # [DOWNWARDS BLACK ARROW] U+E233 -> U+E540
  ["ee88b4", "ee9592"], # [BLACK RIGHTWARDS ARROW] U+E234 -> U+E552
  ["ee88b5", "ee9593"], # [LEFTWARDS BLACK ARROW] U+E235 -> U+E553
  ["ee88ba", "ee94ae"], # [BLACK RIGHT-POINTING TRIANGLE] U+E23A -> U+E52E
  ["ee88bb", "ee94ad"], # [BLACK LEFT-POINTING TRIANGLE] U+E23B -> U+E52D
  ["ee88bc", "ee94b0"], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+E23C -> U+E530
  ["ee88bd", "ee94af"], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+E23D -> U+E52F
  ["ee8cb2", "eeaaad"], # [HEAVY LARGE CIRCLE] U+E332 -> U+EAAD
  ["ee8cb3", "ee9590"], # [CROSS MARK] U+E333 -> U+E550
  ["ee80a1", "ee9282"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E021 -> U+E482
  ["ee80a0", "ee9283"], # [BLACK QUESTION MARK ORNAMENT] U+E020 -> U+E483
  ["ee8cb6", "ee9283"], # [WHITE QUESTION MARK ORNAMENT] U+E336 -> U+E483
  ["ee8cb7", "ee9282"], # [WHITE EXCLAMATION MARK ORNAMENT] U+E337 -> U+E482
  ["ee8891", :undef], # [DOUBLE CURLY LOOP] U+E211 -> "[フリーダイヤル]"
  ["ee80a2", "ee9695"], # [HEAVY BLACK HEART] U+E022 -> U+E595
  ["ee8ca7", "eeadb5"], # [BEATING HEART] U+E327 -> U+EB75
  ["ee80a3", "ee91b7"], # [BROKEN HEART] U+E023 -> U+E477
  ["ee8ca8", "eeadb5"], # [GROWING HEART] U+E328 -> U+EB75
  ["ee8ca9", "ee93aa"], # [HEART WITH ARROW] U+E329 -> U+E4EA
  ["ee8caa", "eeaaa7"], # [BLUE HEART] U+E32A -> U+EAA7
  ["ee8cab", "eeaaa8"], # [GREEN HEART] U+E32B -> U+EAA8
  ["ee8cac", "eeaaa9"], # [YELLOW HEART] U+E32C -> U+EAA9
  ["ee8cad", "eeaaaa"], # [PURPLE HEART] U+E32D -> U+EAAA
  ["ee90b7", "eead94"], # [HEART WITH RIBBON] U+E437 -> U+EB54
  ["ee8884", "ee9695"], # [HEART DECORATION] U+E204 -> U+E595
  ["ee888c", "eeaaa5"], # [BLACK HEART SUIT] U+E20C -> U+EAA5
  ["ee888e", "ee96a1"], # [BLACK SPADE SUIT] U+E20E -> U+E5A1
  ["ee888d", "ee96a2"], # [BLACK DIAMOND SUIT] U+E20D -> U+E5A2
  ["ee888f", "ee96a3"], # [BLACK CLUB SUIT] U+E20F -> U+E5A3
  ["ee8c8e", "ee91bd"], # [SMOKING SYMBOL] U+E30E -> U+E47D
  ["ee8888", "ee91be"], # [NO SMOKING SYMBOL] U+E208 -> U+E47E
  ["ee888a", "ee91bf"], # [WHEELCHAIR SYMBOL] U+E20A -> U+E47F
  ["ee8992", "ee9281"], # [WARNING SIGN] U+E252 -> U+E481
  ["ee84b6", "ee92ae"], # [BICYCLE] U+E136 -> U+E4AE
  ["ee8881", "eeadb2"], # [PEDESTRIAN] U+E201 -> U+EB72
  ["ee84b8", :undef], # [MENS SYMBOL] U+E138 -> "[♂]"
  ["ee84b9", :undef], # [WOMENS SYMBOL] U+E139 -> "[♀]"
  ["ee84bf", "ee9798"], # [BATH] U+E13F -> U+E5D8
  ["ee8591", "ee92a5"], # [RESTROOM] U+E151 -> U+E4A5
  ["ee8580", "ee92a5"], # [TOILET] U+E140 -> U+E4A5
  ["ee8c89", "ee92a5"], # [WATER CLOSET] U+E309 -> U+E4A5
  ["ee84ba", "eeac98"], # [BABY SYMBOL] U+E13A -> U+EB18
  ["ee8894", "eeaa85"], # [SQUARED COOL] U+E214 -> U+EA85
  ["ee88a9", "eeaa88"], # [SQUARED ID] U+E229 -> U+EA88
  ["ee8892", "ee96b5"], # [SQUARED NEW] U+E212 -> U+E5B5
  ["ee898d", "ee96ad"], # [SQUARED OK] U+E24D -> U+E5AD
  ["ee8893", "ee948f"], # [SQUARED UP WITH EXCLAMATION MARK] U+E213 -> U+E50F
  ["ee84ae", "ee9792"], # [SQUARED VS] U+E12E -> U+E5D2
  ["ee8883", :undef], # [SQUARED KATAKANA KOKO] U+E203 -> "[ココ]"
  ["ee88a8", "eeaa87"], # [SQUARED KATAKANA SA] U+E228 -> U+EA87
  ["ee88ab", "eeaa8a"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+E22B -> U+EA8A
  ["ee88aa", "eeaa89"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+E22A -> U+EA89
  ["ee8895", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6709] U+E215 -> "[有]"
  ["ee8896", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7121] U+E216 -> "[無]"
  ["ee8897", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6708] U+E217 -> "[月]"
  ["ee8898", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7533] U+E218 -> "[申]"
  ["ee88a7", "eeaa86"], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+E227 -> U+EA86
  ["ee88ac", "eeaa8b"], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+E22C -> U+EA8B
  ["ee88ad", "eeaa8c"], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+E22D -> U+EA8C
  ["ee8c95", "ee93b1"], # [CIRCLED IDEOGRAPH SECRET] U+E315 -> U+E4F1
  ["ee8c8d", "eeaa99"], # [CIRCLED IDEOGRAPH CONGRATULATION] U+E30D -> U+EA99
  ["ee88a6", "ee93b7"], # [CIRCLED IDEOGRAPH ADVANTAGE] U+E226 -> U+E4F7
  ["ee848f", "ee91b6"], # [ELECTRIC LIGHT BULB] U+E10F -> U+E476
  ["ee8cb4", "ee93a5"], # [ANGER SYMBOL] U+E334 -> U+E4E5
  ["ee8c91", "ee91ba"], # [BOMB] U+E311 -> U+E47A
  ["ee84bc", "ee91b5"], # [SLEEPING SYMBOL] U+E13C -> U+E475
  ["ee8cb1", "ee96b1"], # [SPLASHING SWEAT SYMBOL] U+E331 -> U+E5B1
  ["ee8cb0", "ee93b4"], # [DASH SYMBOL] U+E330 -> U+E4F4
  ["ee819a", "ee93b5"], # [PILE OF POO] U+E05A -> U+E4F5
  ["ee858c", "ee93a9"], # [FLEXED BICEPS] U+E14C -> U+E4E9
  ["ee8cae", "eeaaab"], # [SPARKLES] U+E32E -> U+EAAB
  ["ee8885", "ee91b9"], # [EIGHT POINTED BLACK STAR] U+E205 -> U+E479
  ["ee8886", "ee94be"], # [EIGHT SPOKED ASTERISK] U+E206 -> U+E53E
  ["ee8899", "ee958a"], # [LARGE RED CIRCLE] U+E219 -> U+E54A
  ["ee889a", "ee958b"], # [BLACK SQUARE BUTTON] U+E21A -> U+E54B
  ["ee889b", "ee958b"], # [WHITE SQUARE BUTTON] U+E21B -> U+E54B
  ["ee8caf", "ee928b"], # [WHITE MEDIUM STAR] U+E32F -> U+E48B
  ["ee8581", "ee9491"], # [SPEAKER WITH THREE SOUND WAVES] U+E141 -> U+E511
  ["ee8494", "ee9498"], # [LEFT-POINTING MAGNIFYING GLASS] U+E114 -> U+E518
  ["ee8584", "ee949c"], # [LOCK] U+E144 -> U+E51C
  ["ee8585", "ee949c"], # [OPEN LOCK] U+E145 -> U+E51C
  ["ee80bf", "ee9499"], # [KEY] U+E03F -> U+E519
  ["ee8ca5", "ee9492"], # [BELL] U+E325 -> U+E512
  ["ee898c", :undef], # [TOP WITH UPWARDS ARROW ABOVE] U+E24C -> "[TOP]"
  ["ee8090", "eeae83"], # [RAISED FIST] U+E010 -> U+EB83
  ["ee8092", "ee96a7"], # [RAISED HAND] U+E012 -> U+E5A7
  ["ee8091", "ee96a6"], # [VICTORY HAND] U+E011 -> U+E5A6
  ["ee808d", "ee93b3"], # [FISTED HAND SIGN] U+E00D -> U+E4F3
  ["ee808e", "ee93b9"], # [THUMBS UP SIGN] U+E00E -> U+E4F9
  ["ee808f", "ee93b6"], # [WHITE UP POINTING INDEX] U+E00F -> U+E4F6
  ["ee88ae", "eeaa8d"], # [WHITE UP POINTING BACKHAND INDEX] U+E22E -> U+EA8D
  ["ee88af", "eeaa8e"], # [WHITE DOWN POINTING BACKHAND INDEX] U+E22F -> U+EA8E
  ["ee88b0", "ee93bf"], # [WHITE LEFT POINTING BACKHAND INDEX] U+E230 -> U+E4FF
  ["ee88b1", "ee9480"], # [WHITE RIGHT POINTING BACKHAND INDEX] U+E231 -> U+E500
  ["ee909e", "eeab96"], # [WAVING HAND SIGN] U+E41E -> U+EAD6
  ["ee909f", "eeab93"], # [CLAPPING HANDS SIGN] U+E41F -> U+EAD3
  ["ee90a0", "eeab94"], # [OK HAND SIGN] U+E420 -> U+EAD4
  ["ee90a1", "eeab95"], # [THUMBS DOWN SIGN] U+E421 -> U+EAD5
  ["ee90a2", "eeab96"], # [OPEN HANDS SIGN] U+E422 -> U+EAD6
  ["ee94b8", :undef], # [EMOJI COMPATIBILITY SYMBOL-50] U+E538 -> U+3013 (GETA)
  ["ee94b9", :undef], # [EMOJI COMPATIBILITY SYMBOL-51] U+E539 -> U+3013 (GETA)
  ["ee94ba", :undef], # [EMOJI COMPATIBILITY SYMBOL-52] U+E53A -> U+3013 (GETA)
  ["ee94bb", :undef], # [EMOJI COMPATIBILITY SYMBOL-53] U+E53B -> U+3013 (GETA)
  ["ee94bc", :undef], # [EMOJI COMPATIBILITY SYMBOL-54] U+E53C -> "[v"
  ["ee94bd", :undef], # [EMOJI COMPATIBILITY SYMBOL-55] U+E53D -> "oda"
  ["ee94be", :undef], # [EMOJI COMPATIBILITY SYMBOL-56] U+E53E -> "fone]"
  ["ee8994", :undef], # [EMOJI COMPATIBILITY SYMBOL-57] U+E254 -> U+3013 (GETA)
  ["ee8995", :undef], # [EMOJI COMPATIBILITY SYMBOL-58] U+E255 -> U+3013 (GETA)
  ["ee8996", :undef], # [EMOJI COMPATIBILITY SYMBOL-59] U+E256 -> U+3013 (GETA)
  ["ee8997", :undef], # [EMOJI COMPATIBILITY SYMBOL-60] U+E257 -> U+3013 (GETA)
  ["ee8998", :undef], # [EMOJI COMPATIBILITY SYMBOL-61] U+E258 -> U+3013 (GETA)
  ["ee8999", :undef], # [EMOJI COMPATIBILITY SYMBOL-62] U+E259 -> U+3013 (GETA)
  ["ee899a", :undef], # [EMOJI COMPATIBILITY SYMBOL-63] U+E25A -> U+3013 (GETA)
  ["ee948a", :undef], # [EMOJI COMPATIBILITY SYMBOL-64] U+E50A -> U+3013 (GETA)
]

EMOJI_EXCHANGE_TBL['UTF8-SoftBank']['UTF-8'] = [
  ["ee818a", "e29880"], # [BLACK SUN WITH RAYS] U+E04A -> U+2600
  ["ee8189", "e29881"], # [CLOUD] U+E049 -> U+2601
  ["ee818b", "e29894"], # [UMBRELLA WITH RAIN DROPS] U+E04B -> U+2614
  ["ee8188", "e29b84"], # [SNOWMAN WITHOUT SNOW] U+E048 -> U+26C4
  ["ee84bd", "e29aa1"], # [HIGH VOLTAGE SIGN] U+E13D -> U+26A1
  ["ee9183", :undef], # [CYCLONE] U+E443 -> U+1F300
  ["ee90bc", :undef], # [CLOSED UMBRELLA] U+E43C -> U+1F302
  ["ee918b", :undef], # [NIGHT WITH STARS] U+E44B -> U+1F303
  ["ee818d", :undef], # [SUNRISE OVER MOUNTAINS] U+E04D -> U+1F304
  ["ee9189", :undef], # [SUNRISE] U+E449 -> U+1F305
  ["ee8586", :undef], # [CITYSCAPE AT DUSK] U+E146 -> "[夕焼け]"
  ["ee918a", :undef], # [SUNSET OVER BUILDINGS] U+E44A -> U+1F307
  ["ee918c", :undef], # [RAINBOW] U+E44C -> U+1F308
  ["ee90be", :undef], # [WATER WAVE] U+E43E -> U+1F30A
  ["ee818c", :undef], # [CRESCENT MOON] U+E04C -> U+1F319
  ["ee8cb5", :undef], # [GLOWING STAR] U+E335 -> U+1F31F
  ["ee80a4", :undef], # [CLOCK FACE ONE OCLOCK] U+E024 -> U+1F550
  ["ee80a5", :undef], # [CLOCK FACE TWO OCLOCK] U+E025 -> U+1F551
  ["ee80a6", :undef], # [CLOCK FACE THREE OCLOCK] U+E026 -> U+1F552
  ["ee80a7", :undef], # [CLOCK FACE FOUR OCLOCK] U+E027 -> U+1F553
  ["ee80a8", :undef], # [CLOCK FACE FIVE OCLOCK] U+E028 -> U+1F554
  ["ee80a9", :undef], # [CLOCK FACE SIX OCLOCK] U+E029 -> U+1F555
  ["ee80aa", :undef], # [CLOCK FACE SEVEN OCLOCK] U+E02A -> U+1F556
  ["ee80ab", :undef], # [CLOCK FACE EIGHT OCLOCK] U+E02B -> U+1F557
  ["ee80ac", :undef], # [CLOCK FACE NINE OCLOCK] U+E02C -> U+1F558
  ["ee80ad", :undef], # [CLOCK FACE TEN OCLOCK] U+E02D -> U+1F559
  ["ee80ae", :undef], # [CLOCK FACE ELEVEN OCLOCK] U+E02E -> U+1F55A
  ["ee80af", :undef], # [CLOCK FACE TWELVE OCLOCK] U+E02F -> U+1F55B
  ["ee88bf", "e29988"], # [ARIES] U+E23F -> U+2648
  ["ee8980", "e29989"], # [TAURUS] U+E240 -> U+2649
  ["ee8981", "e2998a"], # [GEMINI] U+E241 -> U+264A
  ["ee8982", "e2998b"], # [CANCER] U+E242 -> U+264B
  ["ee8983", "e2998c"], # [LEO] U+E243 -> U+264C
  ["ee8984", "e2998d"], # [VIRGO] U+E244 -> U+264D
  ["ee8985", "e2998e"], # [LIBRA] U+E245 -> U+264E
  ["ee8986", "e2998f"], # [SCORPIUS] U+E246 -> U+264F
  ["ee8987", "e29990"], # [SAGITTARIUS] U+E247 -> U+2650
  ["ee8988", "e29991"], # [CAPRICORN] U+E248 -> U+2651
  ["ee8989", "e29992"], # [AQUARIUS] U+E249 -> U+2652
  ["ee898a", "e29993"], # [PISCES] U+E24A -> U+2653
  ["ee898b", :undef], # [OPHIUCHUS] U+E24B -> U+26CE
  ["ee8490", :undef], # [FOUR LEAF CLOVER] U+E110 -> U+1F340
  ["ee8c84", :undef], # [TULIP] U+E304 -> U+1F337
  ["ee8498", :undef], # [MAPLE LEAF] U+E118 -> U+1F341
  ["ee80b0", :undef], # [CHERRY BLOSSOM] U+E030 -> U+1F338
  ["ee80b2", :undef], # [ROSE] U+E032 -> U+1F339
  ["ee8499", :undef], # [FALLEN LEAF] U+E119 -> U+1F342
  ["ee9187", :undef], # [LEAF FLUTTERING IN WIND] U+E447 -> U+1F343
  ["ee8c83", :undef], # [HIBISCUS] U+E303 -> U+1F33A
  ["ee8c85", :undef], # [SUNFLOWER] U+E305 -> U+1F33B
  ["ee8c87", :undef], # [PALM TREE] U+E307 -> U+1F334
  ["ee8c88", :undef], # [CACTUS] U+E308 -> U+1F335
  ["ee9184", :undef], # [EAR OF RICE] U+E444 -> U+1F33E
  ["ee8d85", :undef], # [RED APPLE] U+E345 -> U+1F34E
  ["ee8d86", :undef], # [TANGERINE] U+E346 -> U+1F34A
  ["ee8d87", :undef], # [STRAWBERRY] U+E347 -> U+1F353
  ["ee8d88", :undef], # [WATERMELON] U+E348 -> U+1F349
  ["ee8d89", :undef], # [TOMATO] U+E349 -> U+1F345
  ["ee8d8a", :undef], # [AUBERGINE] U+E34A -> U+1F346
  ["ee9099", :undef], # [EYES] U+E419 -> U+1F440
  ["ee909b", :undef], # [EAR] U+E41B -> U+1F442
  ["ee909a", :undef], # [NOSE] U+E41A -> U+1F443
  ["ee909c", :undef], # [MOUTH] U+E41C -> U+1F444
  ["ee8c9c", :undef], # [LIPSTICK] U+E31C -> U+1F484
  ["ee8c9d", :undef], # [NAIL POLISH] U+E31D -> U+1F485
  ["ee8c9e", :undef], # [FACE MASSAGE] U+E31E -> U+1F486
  ["ee8c9f", :undef], # [HAIRCUT] U+E31F -> U+1F487
  ["ee8ca0", :undef], # [BARBER POLE] U+E320 -> U+1F488
  ["ee8081", :undef], # [BOY] U+E001 -> U+1F466
  ["ee8082", :undef], # [GIRL] U+E002 -> U+1F467
  ["ee8084", :undef], # [MAN] U+E004 -> U+1F468
  ["ee8085", :undef], # [WOMAN] U+E005 -> U+1F469
  ["ee90a8", :undef], # [MAN AND WOMAN HOLDING HANDS] U+E428 -> U+1F46B
  ["ee8592", :undef], # [POLICE OFFICER] U+E152 -> U+1F46E
  ["ee90a9", :undef], # [WOMAN WITH BUNNY EARS] U+E429 -> U+1F46F
  ["ee9495", :undef], # [WESTERN PERSON] U+E515 -> U+1F471
  ["ee9496", :undef], # [MAN WITH GUA PI MAO] U+E516 -> U+1F472
  ["ee9497", :undef], # [MAN WITH TURBAN] U+E517 -> U+1F473
  ["ee9498", :undef], # [OLDER MAN] U+E518 -> U+1F474
  ["ee9499", :undef], # [OLDER WOMAN] U+E519 -> U+1F475
  ["ee949a", :undef], # [BABY] U+E51A -> U+1F476
  ["ee949b", :undef], # [CONSTRUCTION WORKER] U+E51B -> U+1F477
  ["ee949c", :undef], # [PRINCESS] U+E51C -> U+1F478
  ["ee849b", :undef], # [GHOST] U+E11B -> U+1F47B
  ["ee818e", :undef], # [BABY ANGEL] U+E04E -> U+1F47C
  ["ee848c", :undef], # [EXTRATERRESTRIAL ALIEN] U+E10C -> U+1F47D
  ["ee84ab", :undef], # [ALIEN MONSTER] U+E12B -> U+1F47E
  ["ee849a", :undef], # [IMP] U+E11A -> U+1F47F
  ["ee849c", :undef], # [SKULL] U+E11C -> U+1F480
  ["ee8993", :undef], # [INFORMATION DESK PERSON] U+E253 -> U+1F481
  ["ee949e", :undef], # [GUARDSMAN] U+E51E -> U+1F482
  ["ee949f", :undef], # [DANCER] U+E51F -> U+1F483
  ["ee94ad", :undef], # [SNAKE] U+E52D -> U+1F40D
  ["ee84b4", :undef], # [HORSE] U+E134 -> U+1F40E
  ["ee94ae", :undef], # [CHICKEN] U+E52E -> U+1F414
  ["ee94af", :undef], # [BOAR] U+E52F -> U+1F417
  ["ee94b0", :undef], # [BACTRIAN CAMEL] U+E530 -> U+1F42B
  ["ee94a6", :undef], # [ELEPHANT] U+E526 -> U+1F418
  ["ee94a7", :undef], # [KOALA] U+E527 -> U+1F428
  ["ee94a8", :undef], # [MONKEY] U+E528 -> U+1F412
  ["ee94a9", :undef], # [SHEEP] U+E529 -> U+1F411
  ["ee848a", :undef], # [OCTOPUS] U+E10A -> U+1F419
  ["ee9181", :undef], # [SPIRAL SHELL] U+E441 -> U+1F41A
  ["ee94a5", :undef], # [BUG] U+E525 -> U+1F41B
  ["ee94a2", :undef], # [TROPICAL FISH] U+E522 -> U+1F420
  ["ee94a3", :undef], # [BABY CHICK] U+E523 -> U+1F424
  ["ee94a1", :undef], # [BIRD] U+E521 -> U+1F426
  ["ee8195", :undef], # [PENGUIN] U+E055 -> U+1F427
  ["ee8099", :undef], # [FISH] U+E019 -> U+1F41F
  ["ee94a0", :undef], # [DOLPHIN] U+E520 -> U+1F42C
  ["ee8193", :undef], # [MOUSE FACE] U+E053 -> U+1F42D
  ["ee8190", :undef], # [TIGER FACE] U+E050 -> U+1F42F
  ["ee818f", :undef], # [CAT FACE] U+E04F -> U+1F431
  ["ee8194", :undef], # [SPOUTING WHALE] U+E054 -> U+1F433
  ["ee809a", :undef], # [HORSE FACE] U+E01A -> U+1F434
  ["ee8489", :undef], # [MONKEY FACE] U+E109 -> U+1F435
  ["ee8192", :undef], # [DOG FACE] U+E052 -> U+1F436
  ["ee848b", :undef], # [PIG FACE] U+E10B -> U+1F437
  ["ee8191", :undef], # [BEAR FACE] U+E051 -> U+1F43B
  ["ee94a4", :undef], # [HAMSTER FACE] U+E524 -> U+1F439
  ["ee94aa", :undef], # [WOLF FACE] U+E52A -> U+1F43A
  ["ee94ab", :undef], # [COW FACE] U+E52B -> U+1F42E
  ["ee94ac", :undef], # [RABBIT FACE] U+E52C -> U+1F430
  ["ee94b1", :undef], # [FROG FACE] U+E531 -> U+1F438
  ["ee8199", :undef], # [ANGRY FACE] U+E059 -> U+1F600
  ["ee9090", :undef], # [ASTONISHED FACE] U+E410 -> U+1F602
  ["ee8198", :undef], # [DISAPPOINTED FACE] U+E058 -> U+1F603
  ["ee908f", :undef], # [EXASPERATED FACE] U+E40F -> U+1F605
  ["ee908e", :undef], # [EXPRESSIONLESS FACE] U+E40E -> U+1F606
  ["ee8486", :undef], # [FACE WITH HEART-SHAPED EYES] U+E106 -> U+1F607
  ["ee8485", :undef], # [WINKING FACE WITH STUCK-OUT TONGUE] U+E105 -> U+1F609
  ["ee9089", :undef], # [FACE WITH STUCK-OUT TONGUE] U+E409 -> U+1F60A
  ["ee9098", :undef], # [FACE THROWING A KISS] U+E418 -> U+1F60C
  ["ee9097", :undef], # [FACE KISSING] U+E417 -> U+1F60D
  ["ee908c", :undef], # [FACE WITH MASK] U+E40C -> U+1F60E
  ["ee908d", :undef], # [FLUSHED FACE] U+E40D -> U+1F60F
  ["ee8197", :undef], # [HAPPY FACE WITH OPEN MOUTH] U+E057 -> U+1F610
  ["ee9084", :undef], # [HAPPY FACE WITH GRIN] U+E404 -> U+1F613
  ["ee9092", :undef], # [HAPPY AND CRYING FACE] U+E412 -> U+1F614
  ["ee8196", :undef], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+E056 -> U+1F615
  ["ee9094", "e298ba"], # [WHITE SMILING FACE] U+E414 -> U+263A
  ["ee9095", :undef], # [HAPPY FACE WITH OPEN MOUTH AND RAISED EYEBROWS] U+E415 -> U+1F616
  ["ee9093", :undef], # [CRYING FACE] U+E413 -> U+1F617
  ["ee9091", :undef], # [LOUDLY CRYING FACE] U+E411 -> U+1F618
  ["ee908b", :undef], # [FEARFUL FACE] U+E40B -> U+1F619
  ["ee9086", :undef], # [PERSEVERING FACE] U+E406 -> U+1F61A
  ["ee9096", :undef], # [POUTING FACE] U+E416 -> U+1F61B
  ["ee908a", :undef], # [RELIEVED FACE] U+E40A -> U+1F61C
  ["ee9087", :undef], # [CONFOUNDED FACE] U+E407 -> U+1F61D
  ["ee9083", :undef], # [PENSIVE FACE] U+E403 -> U+1F61E
  ["ee8487", :undef], # [FACE SCREAMING IN FEAR] U+E107 -> U+1F61F
  ["ee9088", :undef], # [SLEEPY FACE] U+E408 -> U+1F620
  ["ee9082", :undef], # [SMIRKING FACE] U+E402 -> U+1F621
  ["ee8488", :undef], # [FACE WITH COLD SWEAT] U+E108 -> U+1F622
  ["ee9081", :undef], # [DISAPPOINTED BUT RELIEVED FACE] U+E401 -> U+1F623
  ["ee9085", :undef], # [WINKING FACE] U+E405 -> U+1F625
  ["ee90a3", :undef], # [FACE WITH NO GOOD GESTURE] U+E423 -> U+1F634
  ["ee90a4", :undef], # [FACE WITH OK GESTURE] U+E424 -> U+1F635
  ["ee90a6", :undef], # [PERSON BOWING DEEPLY] U+E426 -> U+1F636
  ["ee90a7", :undef], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+E427 -> U+1F63B
  ["ee909d", :undef], # [PERSON WITH FOLDED HANDS] U+E41D -> U+1F63E
  ["ee80b6", :undef], # [HOUSE BUILDING] U+E036 -> U+1F3E0
  ["ee80b8", :undef], # [OFFICE BUILDING] U+E038 -> U+1F3E2
  ["ee8593", :undef], # [JAPANESE POST OFFICE] U+E153 -> U+1F3E3
  ["ee8595", :undef], # [HOSPITAL] U+E155 -> U+1F3E5
  ["ee858d", :undef], # [BANK] U+E14D -> U+1F3E6
  ["ee8594", :undef], # [AUTOMATED TELLER MACHINE] U+E154 -> U+1F3E7
  ["ee8598", :undef], # [HOTEL] U+E158 -> U+1F3E8
  ["ee9481", :undef], # [LOVE HOTEL] U+E501 -> U+1F3E9
  ["ee8596", :undef], # [CONVENIENCE STORE] U+E156 -> U+1F3EA
  ["ee8597", :undef], # [SCHOOL] U+E157 -> U+1F3EB
  ["ee80b7", "e29baa"], # [CHURCH] U+E037 -> U+26EA
  ["ee84a1", "e29bb2"], # [FOUNTAIN] U+E121 -> U+26F2
  ["ee9484", :undef], # [DEPARTMENT STORE] U+E504 -> U+1F3EC
  ["ee9485", :undef], # [JAPANESE CASTLE] U+E505 -> U+1F3EF
  ["ee9486", :undef], # [EUROPEAN CASTLE] U+E506 -> U+1F3F0
  ["ee9488", :undef], # [FACTORY] U+E508 -> U+1F3ED
  ["ee80bb", :undef], # [MOUNT FUJI] U+E03B -> U+1F5FB
  ["ee9489", :undef], # [TOKYO TOWER] U+E509 -> U+1F5FC
  ["ee949d", :undef], # [STATUE OF LIBERTY] U+E51D -> U+1F5FD
  ["ee8087", :undef], # [ATHLETIC SHOE] U+E007 -> U+1F45F
  ["ee84be", :undef], # [HIGH-HEELED SHOE] U+E13E -> U+1F460
  ["ee8c9a", :undef], # [WOMANS SANDAL] U+E31A -> U+1F461
  ["ee8c9b", :undef], # [WOMANS BOOTS] U+E31B -> U+1F462
  ["ee94b6", :undef], # [FOOTPRINTS] U+E536 -> U+1F463
  ["ee8086", :undef], # [T-SHIRT] U+E006 -> U+1F455
  ["ee848e", :undef], # [CROWN] U+E10E -> U+1F451
  ["ee8c82", :undef], # [NECKTIE] U+E302 -> U+1F454
  ["ee8c98", :undef], # [WOMANS HAT] U+E318 -> U+1F452
  ["ee8c99", :undef], # [DRESS] U+E319 -> U+1F457
  ["ee8ca1", :undef], # [KIMONO] U+E321 -> U+1F458
  ["ee8ca2", :undef], # [BIKINI] U+E322 -> U+1F459
  ["ee8ca3", :undef], # [HANDBAG] U+E323 -> U+1F45C
  ["ee84af", :undef], # [MONEY BAG] U+E12F -> U+1F4B0
  ["ee8589", :undef], # [CURRENCY EXCHANGE] U+E149 -> U+1F4B1
  ["ee858a", :undef], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+E14A -> U+1F4B9
  ["ee9493", "f09f87a8f09f87b3"], # [REGIONAL INDICATOR SYMBOL LETTERS CN] U+E513 -> U+1F1E8 U+1F1F3
  ["ee948e", "f09f87a9f09f87aa"], # [REGIONAL INDICATOR SYMBOL LETTERS DE] U+E50E -> U+1F1E9 U+1F1EA
  ["ee9491", "f09f87aaf09f87b8"], # [REGIONAL INDICATOR SYMBOL LETTERS ES] U+E511 -> U+1F1EA U+1F1F8
  ["ee948d", "f09f87abf09f87b7"], # [REGIONAL INDICATOR SYMBOL LETTERS FR] U+E50D -> U+1F1EB U+1F1F7
  ["ee9490", "f09f87baf09f87b0"], # [REGIONAL INDICATOR SYMBOL LETTERS GB] U+E510 -> U+1F1FA U+1F1F0
  ["ee948f", "f09f87aef09f87b9"], # [REGIONAL INDICATOR SYMBOL LETTERS IT] U+E50F -> U+1F1EE U+1F1F9
  ["ee948b", "f09f87aff09f87b5"], # [REGIONAL INDICATOR SYMBOL LETTERS JP] U+E50B -> U+1F1EF U+1F1F5
  ["ee9494", "f09f87b0f09f87b7"], # [REGIONAL INDICATOR SYMBOL LETTERS KR] U+E514 -> U+1F1F0 U+1F1F7
  ["ee9492", "f09f87b7f09f87ba"], # [REGIONAL INDICATOR SYMBOL LETTERS RU] U+E512 -> U+1F1F7 U+1F1FA
  ["ee948c", "f09f87baf09f87b8"], # [REGIONAL INDICATOR SYMBOL LETTERS US] U+E50C -> U+1F1FA U+1F1F8
  ["ee849d", :undef], # [FIRE] U+E11D -> U+1F525
  ["ee8496", :undef], # [HAMMER] U+E116 -> U+1F528
  ["ee8493", :undef], # [PISTOL] U+E113 -> U+1F52B
  ["ee88be", :undef], # [SIX POINTED STAR WITH MIDDLE DOT] U+E23E -> U+1F52F
  ["ee8889", :undef], # [JAPANESE SYMBOL FOR BEGINNER] U+E209 -> U+1F530
  ["ee80b1", :undef], # [TRIDENT EMBLEM] U+E031 -> U+1F531
  ["ee84bb", :undef], # [SYRINGE] U+E13B -> U+1F489
  ["ee8c8f", :undef], # [PILL] U+E30F -> U+1F48A
  ["ee94b2", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+E532 -> U+1F170
  ["ee94b3", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+E533 -> U+1F171
  ["ee94b4", :undef], # [NEGATIVE SQUARED AB] U+E534 -> U+1F18E
  ["ee94b5", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+E535 -> U+1F17E
  ["ee8c94", :undef], # [RIBBON] U+E314 -> U+1F380
  ["ee8492", :undef], # [WRAPPED PRESENT] U+E112 -> U+1F381
  ["ee8d8b", :undef], # [BIRTHDAY CAKE] U+E34B -> U+1F382
  ["ee80b3", :undef], # [CHRISTMAS TREE] U+E033 -> U+1F384
  ["ee9188", :undef], # [FATHER CHRISTMAS] U+E448 -> U+1F385
  ["ee8583", :undef], # [CROSSED FLAGS] U+E143 -> U+1F38C
  ["ee8497", :undef], # [FIREWORKS] U+E117 -> U+1F386
  ["ee8c90", :undef], # [BALLOON] U+E310 -> U+1F388
  ["ee8c92", :undef], # [PARTY POPPER] U+E312 -> U+1F389
  ["ee90b6", :undef], # [PINE DECORATION] U+E436 -> U+1F38D
  ["ee90b8", :undef], # [JAPANESE DOLLS] U+E438 -> U+1F38E
  ["ee90b9", :undef], # [GRADUATION CAP] U+E439 -> U+1F393
  ["ee90ba", :undef], # [SCHOOL SATCHEL] U+E43A -> U+1F392
  ["ee90bb", :undef], # [CARP STREAMER] U+E43B -> U+1F38F
  ["ee9180", :undef], # [FIREWORK SPARKLER] U+E440 -> U+1F387
  ["ee9182", :undef], # [WIND CHIME] U+E442 -> U+1F390
  ["ee9185", :undef], # [JACK-O-LANTERN] U+E445 -> U+1F383
  ["ee9186", :undef], # [MOON VIEWING CEREMONY] U+E446 -> U+1F391
  ["ee8089", "e2988e"], # [BLACK TELEPHONE] U+E009 -> U+260E
  ["ee808a", :undef], # [MOBILE PHONE] U+E00A -> U+1F4F1
  ["ee8484", :undef], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+E104 -> U+1F4F2
  ["ee8c81", :undef], # [MEMO] U+E301 -> U+1F4DD
  ["ee808b", :undef], # [FAX MACHINE] U+E00B -> U+1F4E0
  ["ee8483", :undef], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+E103 -> U+1F4E9
  ["ee8481", :undef], # [CLOSED MAILBOX WITH RAISED FLAG] U+E101 -> U+1F4EB
  ["ee8482", :undef], # [POSTBOX] U+E102 -> U+1F4EE
  ["ee8582", :undef], # [PUBLIC ADDRESS LOUDSPEAKER] U+E142 -> U+1F4E2
  ["ee8c97", :undef], # [CHEERING MEGAPHONE] U+E317 -> U+1F4E3
  ["ee858b", :undef], # [SATELLITE ANTENNA] U+E14B -> U+1F4E1
  ["ee849f", :undef], # [SEAT] U+E11F -> U+1F4BA
  ["ee808c", :undef], # [PERSONAL COMPUTER] U+E00C -> U+1F4BB
  ["ee849e", :undef], # [BRIEFCASE] U+E11E -> U+1F4BC
  ["ee8c96", :undef], # [MINIDISC] U+E316 -> U+1F4BD
  ["ee84a6", :undef], # [OPTICAL DISC] U+E126 -> U+1F4BF
  ["ee84a7", :undef], # [DVD] U+E127 -> U+1F4C0
  ["ee8c93", "e29c82"], # [BLACK SCISSORS] U+E313 -> U+2702
  ["ee8588", :undef], # [OPEN BOOK] U+E148 -> U+1F4D6
  ["ee8096", "e29abe"], # [BASEBALL] U+E016 -> U+26BE
  ["ee8094", "e29bb3"], # [FLAG IN HOLE] U+E014 -> U+26F3
  ["ee8095", :undef], # [TENNIS RACQUET AND BALL] U+E015 -> U+1F3BE
  ["ee8098", "e29abd"], # [SOCCER BALL] U+E018 -> U+26BD
  ["ee8093", :undef], # [SKI AND SKI BOOT] U+E013 -> U+1F3BF
  ["ee90aa", :undef], # [BASKETBALL AND HOOP] U+E42A -> U+1F3C0
  ["ee84b2", :undef], # [CHEQUERED FLAG] U+E132 -> U+1F3C1
  ["ee8495", :undef], # [RUNNER] U+E115 -> U+1F3C3
  ["ee8097", :undef], # [SURFER] U+E017 -> U+1F3C4
  ["ee84b1", :undef], # [TROPHY] U+E131 -> U+1F3C6
  ["ee90ab", :undef], # [AMERICAN FOOTBALL] U+E42B -> U+1F3C8
  ["ee90ad", :undef], # [SWIMMER] U+E42D -> U+1F3CA
  ["ee809e", :undef], # [TRAIN] U+E01E -> U+1F686
  ["ee90b4", :undef], # [METRO] U+E434 -> U+1F687
  ["ee90b5", :undef], # [HIGH-SPEED TRAIN] U+E435 -> U+1F684
  ["ee809f", :undef], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+E01F -> U+1F685
  ["ee809b", :undef], # [AUTOMOBILE] U+E01B -> U+1F697
  ["ee90ae", :undef], # [RECREATIONAL VEHICLE] U+E42E -> U+1F699
  ["ee8599", :undef], # [ONCOMING BUS] U+E159 -> U+1F68D
  ["ee8590", :undef], # [BUS STOP] U+E150 -> U+1F68F
  ["ee8882", :undef], # [SHIP] U+E202 -> U+1F6A2
  ["ee809d", "e29c88"], # [AIRPLANE] U+E01D -> U+2708
  ["ee809c", "e29bb5"], # [SAILBOAT] U+E01C -> U+26F5
  ["ee80b9", :undef], # [STATION] U+E039 -> U+1F689
  ["ee848d", :undef], # [ROCKET] U+E10D -> U+1F680
  ["ee84b5", :undef], # [SPEEDBOAT] U+E135 -> U+1F6A4
  ["ee859a", :undef], # [TAXI] U+E15A -> U+1F695
  ["ee90af", :undef], # [DELIVERY TRUCK] U+E42F -> U+1F69A
  ["ee90b0", :undef], # [FIRE ENGINE] U+E430 -> U+1F692
  ["ee90b1", :undef], # [AMBULANCE] U+E431 -> U+1F691
  ["ee90b2", :undef], # [POLICE CAR] U+E432 -> U+1F693
  ["ee80ba", "e29bbd"], # [FUEL PUMP] U+E03A -> U+26FD
  ["ee858f", "f09f85bf"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+E14F -> U+1F17F
  ["ee858e", :undef], # [HORIZONTAL TRAFFIC LIGHT] U+E14E -> U+1F6A5
  ["ee84b7", :undef], # [CONSTRUCTION SIGN] U+E137 -> U+1F6A7
  ["ee84a3", "e299a8"], # [HOT SPRINGS] U+E123 -> U+2668
  ["ee84a2", "e29bba"], # [TENT] U+E122 -> U+26FA
  ["ee84a4", :undef], # [FERRIS WHEEL] U+E124 -> U+1F3A1
  ["ee90b3", :undef], # [ROLLER COASTER] U+E433 -> U+1F3A2
  ["ee80bc", :undef], # [MICROPHONE] U+E03C -> U+1F3A4
  ["ee80bd", :undef], # [MOVIE CAMERA] U+E03D -> U+1F3A5
  ["ee9487", :undef], # [CINEMA] U+E507 -> U+1F3A6
  ["ee8c8a", :undef], # [HEADPHONE] U+E30A -> U+1F3A7
  ["ee9482", :undef], # [ARTIST PALETTE] U+E502 -> U+1F3A8
  ["ee9483", :undef], # [TOP HAT] U+E503 -> U+1F3A9
  ["ee84a5", :undef], # [TICKET] U+E125 -> U+1F3AB
  ["ee8ca4", :undef], # [CLAPPER BOARD] U+E324 -> U+1F3AC
  ["ee84ad", "f09f8084"], # [MAHJONG TILE RED DRAGON] U+E12D -> U+1F004
  ["ee84b0", :undef], # [DIRECT HIT] U+E130 -> U+1F3AF
  ["ee84b3", :undef], # [SLOT MACHINE] U+E133 -> U+1F3B0
  ["ee90ac", :undef], # [BILLIARDS] U+E42C -> U+1F3B1
  ["ee80be", :undef], # [MUSICAL NOTE] U+E03E -> U+1F3B5
  ["ee8ca6", :undef], # [MULTIPLE MUSICAL NOTES] U+E326 -> U+1F3B6
  ["ee8180", :undef], # [SAXOPHONE] U+E040 -> U+1F3B7
  ["ee8181", :undef], # [GUITAR] U+E041 -> U+1F3B8
  ["ee8182", :undef], # [TRUMPET] U+E042 -> U+1F3BA
  ["ee84ac", "e380bd"], # [PART ALTERNATION MARK] U+E12C -> U+303D
  ["ee8088", :undef], # [CAMERA] U+E008 -> U+1F4F7
  ["ee84aa", :undef], # [TELEVISION] U+E12A -> U+1F4FA
  ["ee84a8", :undef], # [RADIO] U+E128 -> U+1F4FB
  ["ee84a9", :undef], # [VIDEOCASSETTE] U+E129 -> U+1F4FC
  ["ee8083", :undef], # [KISS MARK] U+E003 -> U+1F48B
  ["ee80b4", :undef], # [RING] U+E034 -> U+1F48D
  ["ee80b5", :undef], # [GEM STONE] U+E035 -> U+1F48E
  ["ee8491", :undef], # [KISS] U+E111 -> U+1F48F
  ["ee8c86", :undef], # [BOUQUET] U+E306 -> U+1F490
  ["ee90a5", :undef], # [COUPLE WITH HEART] U+E425 -> U+1F491
  ["ee90bd", :undef], # [WEDDING] U+E43D -> U+1F492
  ["ee8887", :undef], # [NO ONE UNDER EIGHTEEN SYMBOL] U+E207 -> U+1F51E
  ["ee898e", "c2a9"], # [COPYRIGHT SIGN] U+E24E -> U+A9
  ["ee898f", "c2ae"], # [REGISTERED SIGN] U+E24F -> U+AE
  ["ee94b7", "e284a2"], # [TRADE MARK SIGN] U+E537 -> U+2122
  ["ee8890", "23e283a3"], # [HASH KEY] U+E210 -> U+23 U+20E3
  ["ee889c", "31e283a3"], # [KEYCAP 1] U+E21C -> U+31 U+20E3
  ["ee889d", "32e283a3"], # [KEYCAP 2] U+E21D -> U+32 U+20E3
  ["ee889e", "33e283a3"], # [KEYCAP 3] U+E21E -> U+33 U+20E3
  ["ee889f", "34e283a3"], # [KEYCAP 4] U+E21F -> U+34 U+20E3
  ["ee88a0", "35e283a3"], # [KEYCAP 5] U+E220 -> U+35 U+20E3
  ["ee88a1", "36e283a3"], # [KEYCAP 6] U+E221 -> U+36 U+20E3
  ["ee88a2", "37e283a3"], # [KEYCAP 7] U+E222 -> U+37 U+20E3
  ["ee88a3", "38e283a3"], # [KEYCAP 8] U+E223 -> U+38 U+20E3
  ["ee88a4", "39e283a3"], # [KEYCAP 9] U+E224 -> U+39 U+20E3
  ["ee88a5", "30e283a3"], # [KEYCAP 0] U+E225 -> U+30 U+20E3
  ["ee888b", :undef], # [ANTENNA WITH BARS] U+E20B -> U+1F4F6
  ["ee8990", :undef], # [VIBRATION MODE] U+E250 -> U+1F4F3
  ["ee8991", :undef], # [MOBILE PHONE OFF] U+E251 -> U+1F4F4
  ["ee84a0", :undef], # [HAMBURGER] U+E120 -> U+1F354
  ["ee8d82", :undef], # [RICE BALL] U+E342 -> U+1F359
  ["ee8186", :undef], # [SHORTCAKE] U+E046 -> U+1F370
  ["ee8d80", :undef], # [STEAMING BOWL] U+E340 -> U+1F35C
  ["ee8cb9", :undef], # [BREAD] U+E339 -> U+1F35E
  ["ee8587", :undef], # [COOKING] U+E147 -> U+1F373
  ["ee8cba", :undef], # [SOFT ICE CREAM] U+E33A -> U+1F366
  ["ee8cbb", :undef], # [FRENCH FRIES] U+E33B -> U+1F35F
  ["ee8cbc", :undef], # [DANGO] U+E33C -> U+1F361
  ["ee8cbd", :undef], # [RICE CRACKER] U+E33D -> U+1F358
  ["ee8cbe", :undef], # [COOKED RICE] U+E33E -> U+1F35A
  ["ee8cbf", :undef], # [SPAGHETTI] U+E33F -> U+1F35D
  ["ee8d81", :undef], # [CURRY AND RICE] U+E341 -> U+1F35B
  ["ee8d83", :undef], # [ODEN] U+E343 -> U+1F362
  ["ee8d84", :undef], # [SUSHI] U+E344 -> U+1F363
  ["ee8d8c", :undef], # [BENTO BOX] U+E34C -> U+1F371
  ["ee8d8d", :undef], # [POT OF FOOD] U+E34D -> U+1F372
  ["ee90bf", :undef], # [SHAVED ICE] U+E43F -> U+1F367
  ["ee8183", :undef], # [FORK AND KNIFE] U+E043 -> U+1F374
  ["ee8185", "e29895"], # [HOT BEVERAGE] U+E045 -> U+2615
  ["ee8184", :undef], # [COCKTAIL GLASS] U+E044 -> U+1F378
  ["ee8187", :undef], # [BEER MUG] U+E047 -> U+1F37A
  ["ee8cb8", :undef], # [TEACUP WITHOUT HANDLE] U+E338 -> U+1F375
  ["ee8c8b", :undef], # [SAKE BOTTLE AND CUP] U+E30B -> U+1F376
  ["ee8c8c", :undef], # [CLINKING BEER MUGS] U+E30C -> U+1F37B
  ["ee88b6", "e28697"], # [NORTH EAST ARROW] U+E236 -> U+2197
  ["ee88b8", "e28698"], # [SOUTH EAST ARROW] U+E238 -> U+2198
  ["ee88b7", "e28696"], # [NORTH WEST ARROW] U+E237 -> U+2196
  ["ee88b9", "e28699"], # [SOUTH WEST ARROW] U+E239 -> U+2199
  ["ee88b2", "e2ac86"], # [UPWARDS BLACK ARROW] U+E232 -> U+2B06
  ["ee88b3", "e2ac87"], # [DOWNWARDS BLACK ARROW] U+E233 -> U+2B07
  ["ee88b4", "e29ea1"], # [BLACK RIGHTWARDS ARROW] U+E234 -> U+27A1
  ["ee88b5", "e2ac85"], # [LEFTWARDS BLACK ARROW] U+E235 -> U+2B05
  ["ee88ba", "e296b6"], # [BLACK RIGHT-POINTING TRIANGLE] U+E23A -> U+25B6
  ["ee88bb", "e29780"], # [BLACK LEFT-POINTING TRIANGLE] U+E23B -> U+25C0
  ["ee88bc", :undef], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+E23C -> U+23E9
  ["ee88bd", :undef], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+E23D -> U+23EA
  ["ee8cb2", "e2ad95"], # [HEAVY LARGE CIRCLE] U+E332 -> U+2B55
  ["ee8cb3", :undef], # [CROSS MARK] U+E333 -> U+274C
  ["ee80a1", "e29da2"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+E021 -> U+2762
  ["ee80a0", :undef], # [BLACK QUESTION MARK ORNAMENT] U+E020 -> U+2753
  ["ee8cb6", :undef], # [WHITE QUESTION MARK ORNAMENT] U+E336 -> U+2754
  ["ee8cb7", :undef], # [WHITE EXCLAMATION MARK ORNAMENT] U+E337 -> U+2755
  ["ee8891", :undef], # [DOUBLE CURLY LOOP] U+E211 -> U+27BF
  ["ee80a2", "e29da4"], # [HEAVY BLACK HEART] U+E022 -> U+2764
  ["ee8ca7", :undef], # [BEATING HEART] U+E327 -> U+1F493
  ["ee80a3", :undef], # [BROKEN HEART] U+E023 -> U+1F494
  ["ee8ca8", :undef], # [GROWING HEART] U+E328 -> U+1F497
  ["ee8ca9", :undef], # [HEART WITH ARROW] U+E329 -> U+1F498
  ["ee8caa", :undef], # [BLUE HEART] U+E32A -> U+1F499
  ["ee8cab", :undef], # [GREEN HEART] U+E32B -> U+1F49A
  ["ee8cac", :undef], # [YELLOW HEART] U+E32C -> U+1F49B
  ["ee8cad", :undef], # [PURPLE HEART] U+E32D -> U+1F49C
  ["ee90b7", :undef], # [HEART WITH RIBBON] U+E437 -> U+1F49D
  ["ee8884", :undef], # [HEART DECORATION] U+E204 -> U+1F49F
  ["ee888c", "e299a5"], # [BLACK HEART SUIT] U+E20C -> U+2665
  ["ee888e", "e299a0"], # [BLACK SPADE SUIT] U+E20E -> U+2660
  ["ee888d", "e299a6"], # [BLACK DIAMOND SUIT] U+E20D -> U+2666
  ["ee888f", "e299a3"], # [BLACK CLUB SUIT] U+E20F -> U+2663
  ["ee8c8e", :undef], # [SMOKING SYMBOL] U+E30E -> U+1F6AC
  ["ee8888", :undef], # [NO SMOKING SYMBOL] U+E208 -> U+1F6AD
  ["ee888a", "e299bf"], # [WHEELCHAIR SYMBOL] U+E20A -> U+267F
  ["ee8992", "e29aa0"], # [WARNING SIGN] U+E252 -> U+26A0
  ["ee84b6", :undef], # [BICYCLE] U+E136 -> U+1F6B2
  ["ee8881", :undef], # [PEDESTRIAN] U+E201 -> U+1F6B6
  ["ee84b8", :undef], # [MENS SYMBOL] U+E138 -> U+1F6B9
  ["ee84b9", :undef], # [WOMENS SYMBOL] U+E139 -> U+1F6BA
  ["ee84bf", :undef], # [BATH] U+E13F -> U+1F6C0
  ["ee8591", :undef], # [RESTROOM] U+E151 -> U+1F6BB
  ["ee8580", :undef], # [TOILET] U+E140 -> U+1F6BD
  ["ee8c89", :undef], # [WATER CLOSET] U+E309 -> U+1F6BE
  ["ee84ba", :undef], # [BABY SYMBOL] U+E13A -> U+1F6BC
  ["ee8894", :undef], # [SQUARED COOL] U+E214 -> U+1F192
  ["ee88a9", :undef], # [SQUARED ID] U+E229 -> U+1F194
  ["ee8892", :undef], # [SQUARED NEW] U+E212 -> U+1F195
  ["ee898d", :undef], # [SQUARED OK] U+E24D -> U+1F197
  ["ee8893", :undef], # [SQUARED UP WITH EXCLAMATION MARK] U+E213 -> U+1F199
  ["ee84ae", :undef], # [SQUARED VS] U+E12E -> U+1F19A
  ["ee8883", :undef], # [SQUARED KATAKANA KOKO] U+E203 -> U+1F201
  ["ee88a8", :undef], # [SQUARED KATAKANA SA] U+E228 -> U+1F202
  ["ee88ab", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+E22B -> U+1F233
  ["ee88aa", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+E22A -> U+1F235
  ["ee8895", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6709] U+E215 -> U+1F236
  ["ee8896", "f09f889a"], # [SQUARED CJK UNIFIED IDEOGRAPH-7121] U+E216 -> U+1F21A
  ["ee8897", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6708] U+E217 -> U+1F237
  ["ee8898", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7533] U+E218 -> U+1F238
  ["ee88a7", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+E227 -> U+1F239
  ["ee88ac", "f09f88af"], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+E22C -> U+1F22F
  ["ee88ad", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+E22D -> U+1F23A
  ["ee8c95", "e38a99"], # [CIRCLED IDEOGRAPH SECRET] U+E315 -> U+3299
  ["ee8c8d", "e38a97"], # [CIRCLED IDEOGRAPH CONGRATULATION] U+E30D -> U+3297
  ["ee88a6", :undef], # [CIRCLED IDEOGRAPH ADVANTAGE] U+E226 -> U+1F250
  ["ee848f", :undef], # [ELECTRIC LIGHT BULB] U+E10F -> U+1F4A1
  ["ee8cb4", :undef], # [ANGER SYMBOL] U+E334 -> U+1F4A2
  ["ee8c91", :undef], # [BOMB] U+E311 -> U+1F4A3
  ["ee84bc", :undef], # [SLEEPING SYMBOL] U+E13C -> U+1F4A4
  ["ee8cb1", :undef], # [SPLASHING SWEAT SYMBOL] U+E331 -> U+1F4A6
  ["ee8cb0", :undef], # [DASH SYMBOL] U+E330 -> U+1F4A8
  ["ee819a", :undef], # [PILE OF POO] U+E05A -> U+1F4A9
  ["ee858c", :undef], # [FLEXED BICEPS] U+E14C -> U+1F4AA
  ["ee8cae", :undef], # [SPARKLES] U+E32E -> U+2728
  ["ee8885", "e29cb4"], # [EIGHT POINTED BLACK STAR] U+E205 -> U+2734
  ["ee8886", "e29cb3"], # [EIGHT SPOKED ASTERISK] U+E206 -> U+2733
  ["ee8899", :undef], # [LARGE RED CIRCLE] U+E219 -> U+1F534
  ["ee889a", :undef], # [BLACK SQUARE BUTTON] U+E21A -> U+1F532
  ["ee889b", :undef], # [WHITE SQUARE BUTTON] U+E21B -> U+1F533
  ["ee8caf", "e2ad90"], # [WHITE MEDIUM STAR] U+E32F -> U+2B50
  ["ee8581", :undef], # [SPEAKER WITH THREE SOUND WAVES] U+E141 -> U+1F50A
  ["ee8494", :undef], # [LEFT-POINTING MAGNIFYING GLASS] U+E114 -> U+1F50D
  ["ee8584", :undef], # [LOCK] U+E144 -> U+1F512
  ["ee8585", :undef], # [OPEN LOCK] U+E145 -> U+1F513
  ["ee80bf", :undef], # [KEY] U+E03F -> U+1F511
  ["ee8ca5", :undef], # [BELL] U+E325 -> U+1F514
  ["ee898c", :undef], # [TOP WITH UPWARDS ARROW ABOVE] U+E24C -> U+1F51D
  ["ee8090", :undef], # [RAISED FIST] U+E010 -> U+270A
  ["ee8092", :undef], # [RAISED HAND] U+E012 -> U+270B
  ["ee8091", "e29c8c"], # [VICTORY HAND] U+E011 -> U+270C
  ["ee808d", :undef], # [FISTED HAND SIGN] U+E00D -> U+1F44A
  ["ee808e", :undef], # [THUMBS UP SIGN] U+E00E -> U+1F44D
  ["ee808f", "e2989d"], # [WHITE UP POINTING INDEX] U+E00F -> U+261D
  ["ee88ae", :undef], # [WHITE UP POINTING BACKHAND INDEX] U+E22E -> U+1F446
  ["ee88af", :undef], # [WHITE DOWN POINTING BACKHAND INDEX] U+E22F -> U+1F447
  ["ee88b0", :undef], # [WHITE LEFT POINTING BACKHAND INDEX] U+E230 -> U+1F448
  ["ee88b1", :undef], # [WHITE RIGHT POINTING BACKHAND INDEX] U+E231 -> U+1F449
  ["ee909e", :undef], # [WAVING HAND SIGN] U+E41E -> U+1F44B
  ["ee909f", :undef], # [CLAPPING HANDS SIGN] U+E41F -> U+1F44F
  ["ee90a0", :undef], # [OK HAND SIGN] U+E420 -> U+1F44C
  ["ee90a1", :undef], # [THUMBS DOWN SIGN] U+E421 -> U+1F44E
  ["ee90a2", :undef], # [OPEN HANDS SIGN] U+E422 -> U+1F450
  ["ee94b8", :undef], # [EMOJI COMPATIBILITY SYMBOL-50] U+E538 -> U+3013 (GETA)
  ["ee94b9", :undef], # [EMOJI COMPATIBILITY SYMBOL-51] U+E539 -> U+3013 (GETA)
  ["ee94ba", :undef], # [EMOJI COMPATIBILITY SYMBOL-52] U+E53A -> U+3013 (GETA)
  ["ee94bb", :undef], # [EMOJI COMPATIBILITY SYMBOL-53] U+E53B -> U+3013 (GETA)
  ["ee94bc", :undef], # [EMOJI COMPATIBILITY SYMBOL-54] U+E53C -> "[v"
  ["ee94bd", :undef], # [EMOJI COMPATIBILITY SYMBOL-55] U+E53D -> "oda"
  ["ee94be", :undef], # [EMOJI COMPATIBILITY SYMBOL-56] U+E53E -> "fone]"
  ["ee8994", :undef], # [EMOJI COMPATIBILITY SYMBOL-57] U+E254 -> U+3013 (GETA)
  ["ee8995", :undef], # [EMOJI COMPATIBILITY SYMBOL-58] U+E255 -> U+3013 (GETA)
  ["ee8996", :undef], # [EMOJI COMPATIBILITY SYMBOL-59] U+E256 -> U+3013 (GETA)
  ["ee8997", :undef], # [EMOJI COMPATIBILITY SYMBOL-60] U+E257 -> U+3013 (GETA)
  ["ee8998", :undef], # [EMOJI COMPATIBILITY SYMBOL-61] U+E258 -> U+3013 (GETA)
  ["ee8999", :undef], # [EMOJI COMPATIBILITY SYMBOL-62] U+E259 -> U+3013 (GETA)
  ["ee899a", :undef], # [EMOJI COMPATIBILITY SYMBOL-63] U+E25A -> U+3013 (GETA)
  ["ee948a", :undef], # [EMOJI COMPATIBILITY SYMBOL-64] U+E50A -> U+3013 (GETA)
]

EMOJI_EXCHANGE_TBL['UTF-8']['UTF8-DoCoMo'] = [
  ["e29880", "ee98be"], # [BLACK SUN WITH RAYS] U+2600 -> U+E63E
  ["e29881", "ee98bf"], # [CLOUD] U+2601 -> U+E63F
  ["e29894", "ee9980"], # [UMBRELLA WITH RAIN DROPS] U+2614 -> U+E640
  ["e29b84", "ee9981"], # [SNOWMAN WITHOUT SNOW] U+26C4 -> U+E641
  ["e29aa1", "ee9982"], # [HIGH VOLTAGE SIGN] U+26A1 -> U+E642
  ["f09f8c80", "ee9983"], # [CYCLONE] U+1F300 -> U+E643
  ["f09f8c81", "ee9984"], # [FOGGY] U+1F301 -> U+E644
  ["f09f8c82", "ee9985"], # [CLOSED UMBRELLA] U+1F302 -> U+E645
  ["f09f8c83", "ee9ab3"], # [NIGHT WITH STARS] U+1F303 -> U+E6B3
  ["f09f8c84", "ee98be"], # [SUNRISE OVER MOUNTAINS] U+1F304 -> U+E63E
  ["f09f8c85", "ee98be"], # [SUNRISE] U+1F305 -> U+E63E
  ["f09f8c87", "ee98be"], # [SUNSET OVER BUILDINGS] U+1F307 -> U+E63E
  ["f09f8c88", :undef], # [RAINBOW] U+1F308 -> "[虹]"
  ["e29d84", :undef], # [SNOWFLAKE] U+2744 -> "[雪結晶]"
  ["e29b85", "ee98beee98bf"], # [SUN BEHIND CLOUD] U+26C5 -> U+E63E U+E63F
  ["f09f8c89", "ee9ab3"], # [BRIDGE AT NIGHT] U+1F309 -> U+E6B3
  ["f09f8c8a", "ee9cbf"], # [WATER WAVE] U+1F30A -> U+E73F
  ["f09f8c8b", :undef], # [VOLCANO] U+1F30B -> "[火山]"
  ["f09f8c8c", "ee9ab3"], # [MILKY WAY] U+1F30C -> U+E6B3
  ["f09f8c8f", :undef], # [EARTH GLOBE ASIA-AUSTRALIA] U+1F30F -> "[地球]"
  ["f09f8c91", "ee9a9c"], # [NEW MOON SYMBOL] U+1F311 -> U+E69C
  ["f09f8c94", "ee9a9d"], # [WAXING GIBBOUS MOON SYMBOL] U+1F314 -> U+E69D
  ["f09f8c93", "ee9a9e"], # [FIRST QUARTER MOON SYMBOL] U+1F313 -> U+E69E
  ["f09f8c99", "ee9a9f"], # [CRESCENT MOON] U+1F319 -> U+E69F
  ["f09f8c95", "ee9aa0"], # [FULL MOON SYMBOL] U+1F315 -> U+E6A0
  ["f09f8c9b", "ee9a9e"], # [FIRST QUARTER MOON WITH FACE] U+1F31B -> U+E69E
  ["f09f8c9f", :undef], # [GLOWING STAR] U+1F31F -> "[☆]"
  ["f09f8ca0", :undef], # [SHOOTING STAR] U+1F320 -> "☆彡"
  ["f09f9590", "ee9aba"], # [CLOCK FACE ONE OCLOCK] U+1F550 -> U+E6BA
  ["f09f9591", "ee9aba"], # [CLOCK FACE TWO OCLOCK] U+1F551 -> U+E6BA
  ["f09f9592", "ee9aba"], # [CLOCK FACE THREE OCLOCK] U+1F552 -> U+E6BA
  ["f09f9593", "ee9aba"], # [CLOCK FACE FOUR OCLOCK] U+1F553 -> U+E6BA
  ["f09f9594", "ee9aba"], # [CLOCK FACE FIVE OCLOCK] U+1F554 -> U+E6BA
  ["f09f9595", "ee9aba"], # [CLOCK FACE SIX OCLOCK] U+1F555 -> U+E6BA
  ["f09f9596", "ee9aba"], # [CLOCK FACE SEVEN OCLOCK] U+1F556 -> U+E6BA
  ["f09f9597", "ee9aba"], # [CLOCK FACE EIGHT OCLOCK] U+1F557 -> U+E6BA
  ["f09f9598", "ee9aba"], # [CLOCK FACE NINE OCLOCK] U+1F558 -> U+E6BA
  ["f09f9599", "ee9aba"], # [CLOCK FACE TEN OCLOCK] U+1F559 -> U+E6BA
  ["f09f959a", "ee9aba"], # [CLOCK FACE ELEVEN OCLOCK] U+1F55A -> U+E6BA
  ["f09f959b", "ee9aba"], # [CLOCK FACE TWELVE OCLOCK] U+1F55B -> U+E6BA
  ["e28c9a", "ee9c9f"], # [WATCH] U+231A -> U+E71F
  ["e28c9b", "ee9c9c"], # [HOURGLASS] U+231B -> U+E71C
  ["e28fb0", "ee9aba"], # [ALARM CLOCK] U+23F0 -> U+E6BA
  ["e28fb3", "ee9c9c"], # [HOURGLASS WITH FLOWING SAND] U+23F3 -> U+E71C
  ["e29988", "ee9986"], # [ARIES] U+2648 -> U+E646
  ["e29989", "ee9987"], # [TAURUS] U+2649 -> U+E647
  ["e2998a", "ee9988"], # [GEMINI] U+264A -> U+E648
  ["e2998b", "ee9989"], # [CANCER] U+264B -> U+E649
  ["e2998c", "ee998a"], # [LEO] U+264C -> U+E64A
  ["e2998d", "ee998b"], # [VIRGO] U+264D -> U+E64B
  ["e2998e", "ee998c"], # [LIBRA] U+264E -> U+E64C
  ["e2998f", "ee998d"], # [SCORPIUS] U+264F -> U+E64D
  ["e29990", "ee998e"], # [SAGITTARIUS] U+2650 -> U+E64E
  ["e29991", "ee998f"], # [CAPRICORN] U+2651 -> U+E64F
  ["e29992", "ee9990"], # [AQUARIUS] U+2652 -> U+E650
  ["e29993", "ee9991"], # [PISCES] U+2653 -> U+E651
  ["e29b8e", :undef], # [OPHIUCHUS] U+26CE -> "[蛇使座]"
  ["f09f8d80", "ee9d81"], # [FOUR LEAF CLOVER] U+1F340 -> U+E741
  ["f09f8cb7", "ee9d83"], # [TULIP] U+1F337 -> U+E743
  ["f09f8cb1", "ee9d86"], # [SEEDLING] U+1F331 -> U+E746
  ["f09f8d81", "ee9d87"], # [MAPLE LEAF] U+1F341 -> U+E747
  ["f09f8cb8", "ee9d88"], # [CHERRY BLOSSOM] U+1F338 -> U+E748
  ["f09f8cb9", :undef], # [ROSE] U+1F339 -> "[バラ]"
  ["f09f8d82", "ee9d87"], # [FALLEN LEAF] U+1F342 -> U+E747
  ["f09f8d83", :undef], # [LEAF FLUTTERING IN WIND] U+1F343 -> "[風に舞う葉]"
  ["f09f8cba", :undef], # [HIBISCUS] U+1F33A -> "[ハイビスカス]"
  ["f09f8cbb", :undef], # [SUNFLOWER] U+1F33B -> "[ひまわり]"
  ["f09f8cb4", :undef], # [PALM TREE] U+1F334 -> "[ヤシ]"
  ["f09f8cb5", :undef], # [CACTUS] U+1F335 -> "[サボテン]"
  ["f09f8cbe", :undef], # [EAR OF RICE] U+1F33E -> "[稲穂]"
  ["f09f8cbd", :undef], # [EAR OF MAIZE] U+1F33D -> "[とうもろこし]"
  ["f09f8d84", :undef], # [MUSHROOM] U+1F344 -> "[キノコ]"
  ["f09f8cb0", :undef], # [CHESTNUT] U+1F330 -> "[栗]"
  ["f09f8cbc", :undef], # [BLOSSOM] U+1F33C -> "[花]"
  ["f09f8cbf", "ee9d81"], # [HERB] U+1F33F -> U+E741
  ["f09f8d92", "ee9d82"], # [CHERRIES] U+1F352 -> U+E742
  ["f09f8d8c", "ee9d84"], # [BANANA] U+1F34C -> U+E744
  ["f09f8d8e", "ee9d85"], # [RED APPLE] U+1F34E -> U+E745
  ["f09f8d8a", :undef], # [TANGERINE] U+1F34A -> "[みかん]"
  ["f09f8d93", :undef], # [STRAWBERRY] U+1F353 -> "[イチゴ]"
  ["f09f8d89", :undef], # [WATERMELON] U+1F349 -> "[スイカ]"
  ["f09f8d85", :undef], # [TOMATO] U+1F345 -> "[トマト]"
  ["f09f8d86", :undef], # [AUBERGINE] U+1F346 -> "[ナス]"
  ["f09f8d88", :undef], # [MELON] U+1F348 -> "[メロン]"
  ["f09f8d8d", :undef], # [PINEAPPLE] U+1F34D -> "[パイナップル]"
  ["f09f8d87", :undef], # [GRAPES] U+1F347 -> "[ブドウ]"
  ["f09f8d91", :undef], # [PEACH] U+1F351 -> "[モモ]"
  ["f09f8d8f", "ee9d85"], # [GREEN APPLE] U+1F34F -> U+E745
  ["f09f9180", "ee9a91"], # [EYES] U+1F440 -> U+E691
  ["f09f9182", "ee9a92"], # [EAR] U+1F442 -> U+E692
  ["f09f9183", :undef], # [NOSE] U+1F443 -> "[鼻]"
  ["f09f9184", "ee9bb9"], # [MOUTH] U+1F444 -> U+E6F9
  ["f09f9185", "ee9ca8"], # [TONGUE] U+1F445 -> U+E728
  ["f09f9284", "ee9c90"], # [LIPSTICK] U+1F484 -> U+E710
  ["f09f9285", :undef], # [NAIL POLISH] U+1F485 -> "[マニキュア]"
  ["f09f9286", :undef], # [FACE MASSAGE] U+1F486 -> "[エステ]"
  ["f09f9287", "ee99b5"], # [HAIRCUT] U+1F487 -> U+E675
  ["f09f9288", :undef], # [BARBER POLE] U+1F488 -> "[床屋]"
  ["f09f91a4", "ee9ab1"], # [BUST IN SILHOUETTE] U+1F464 -> U+E6B1
  ["f09f91a6", "ee9bb0"], # [BOY] U+1F466 -> U+E6F0
  ["f09f91a7", "ee9bb0"], # [GIRL] U+1F467 -> U+E6F0
  ["f09f91a8", "ee9bb0"], # [MAN] U+1F468 -> U+E6F0
  ["f09f91a9", "ee9bb0"], # [WOMAN] U+1F469 -> U+E6F0
  ["f09f91aa", :undef], # [FAMILY] U+1F46A -> "[家族]"
  ["f09f91ab", :undef], # [MAN AND WOMAN HOLDING HANDS] U+1F46B -> "[カップル]"
  ["f09f91ae", :undef], # [POLICE OFFICER] U+1F46E -> "[警官]"
  ["f09f91af", :undef], # [WOMAN WITH BUNNY EARS] U+1F46F -> "[バニー]"
  ["f09f91b0", :undef], # [BRIDE WITH VEIL] U+1F470 -> "[花嫁]"
  ["f09f91b1", :undef], # [WESTERN PERSON] U+1F471 -> "[白人]"
  ["f09f91b2", :undef], # [MAN WITH GUA PI MAO] U+1F472 -> "[中国人]"
  ["f09f91b3", :undef], # [MAN WITH TURBAN] U+1F473 -> "[インド人]"
  ["f09f91b4", :undef], # [OLDER MAN] U+1F474 -> "[おじいさん]"
  ["f09f91b5", :undef], # [OLDER WOMAN] U+1F475 -> "[おばあさん]"
  ["f09f91b6", :undef], # [BABY] U+1F476 -> "[赤ちゃん]"
  ["f09f91b7", :undef], # [CONSTRUCTION WORKER] U+1F477 -> "[工事現場の人]"
  ["f09f91b8", :undef], # [PRINCESS] U+1F478 -> "[お姫様]"
  ["f09f91b9", :undef], # [JAPANESE OGRE] U+1F479 -> "[なまはげ]"
  ["f09f91ba", :undef], # [JAPANESE GOBLIN] U+1F47A -> "[天狗]"
  ["f09f91bb", :undef], # [GHOST] U+1F47B -> "[お化け]"
  ["f09f91bc", :undef], # [BABY ANGEL] U+1F47C -> "[天使]"
  ["f09f91bd", :undef], # [EXTRATERRESTRIAL ALIEN] U+1F47D -> "[UFO]"
  ["f09f91be", :undef], # [ALIEN MONSTER] U+1F47E -> "[宇宙人]"
  ["f09f91bf", :undef], # [IMP] U+1F47F -> "[アクマ]"
  ["f09f9280", :undef], # [SKULL] U+1F480 -> "[ドクロ]"
  ["f09f9281", :undef], # [INFORMATION DESK PERSON] U+1F481 -> "[案内]"
  ["f09f9282", :undef], # [GUARDSMAN] U+1F482 -> "[衛兵]"
  ["f09f9283", :undef], # [DANCER] U+1F483 -> "[ダンス]"
  ["f09f908c", "ee9d8e"], # [SNAIL] U+1F40C -> U+E74E
  ["f09f908d", :undef], # [SNAKE] U+1F40D -> "[ヘビ]"
  ["f09f908e", "ee9d94"], # [HORSE] U+1F40E -> U+E754
  ["f09f9094", :undef], # [CHICKEN] U+1F414 -> "[ニワトリ]"
  ["f09f9097", :undef], # [BOAR] U+1F417 -> "[イノシシ]"
  ["f09f90ab", :undef], # [BACTRIAN CAMEL] U+1F42B -> "[ラクダ]"
  ["f09f9098", :undef], # [ELEPHANT] U+1F418 -> "[ゾウ]"
  ["f09f90a8", :undef], # [KOALA] U+1F428 -> "[コアラ]"
  ["f09f9092", :undef], # [MONKEY] U+1F412 -> "[サル]"
  ["f09f9091", :undef], # [SHEEP] U+1F411 -> "[ヒツジ]"
  ["f09f9099", :undef], # [OCTOPUS] U+1F419 -> "[タコ]"
  ["f09f909a", :undef], # [SPIRAL SHELL] U+1F41A -> "[巻貝]"
  ["f09f909b", :undef], # [BUG] U+1F41B -> "[ゲジゲジ]"
  ["f09f909c", :undef], # [ANT] U+1F41C -> "[アリ]"
  ["f09f909d", :undef], # [HONEYBEE] U+1F41D -> "[ミツバチ]"
  ["f09f909e", :undef], # [LADY BEETLE] U+1F41E -> "[てんとう虫]"
  ["f09f90a0", "ee9d91"], # [TROPICAL FISH] U+1F420 -> U+E751
  ["f09f90a1", "ee9d91"], # [BLOWFISH] U+1F421 -> U+E751
  ["f09f90a2", :undef], # [TURTLE] U+1F422 -> "[カメ]"
  ["f09f90a4", "ee9d8f"], # [BABY CHICK] U+1F424 -> U+E74F
  ["f09f90a5", "ee9d8f"], # [FRONT-FACING BABY CHICK] U+1F425 -> U+E74F
  ["f09f90a6", "ee9d8f"], # [BIRD] U+1F426 -> U+E74F
  ["f09f90a3", "ee9d8f"], # [HATCHING CHICK] U+1F423 -> U+E74F
  ["f09f90a7", "ee9d90"], # [PENGUIN] U+1F427 -> U+E750
  ["f09f90a9", "ee9aa1"], # [POODLE] U+1F429 -> U+E6A1
  ["f09f909f", "ee9d91"], # [FISH] U+1F41F -> U+E751
  ["f09f90ac", :undef], # [DOLPHIN] U+1F42C -> "[イルカ]"
  ["f09f90ad", :undef], # [MOUSE FACE] U+1F42D -> "[ネズミ]"
  ["f09f90af", :undef], # [TIGER FACE] U+1F42F -> "[トラ]"
  ["f09f90b1", "ee9aa2"], # [CAT FACE] U+1F431 -> U+E6A2
  ["f09f90b3", :undef], # [SPOUTING WHALE] U+1F433 -> "[クジラ]"
  ["f09f90b4", "ee9d94"], # [HORSE FACE] U+1F434 -> U+E754
  ["f09f90b5", :undef], # [MONKEY FACE] U+1F435 -> "[サル]"
  ["f09f90b6", "ee9aa1"], # [DOG FACE] U+1F436 -> U+E6A1
  ["f09f90b7", "ee9d95"], # [PIG FACE] U+1F437 -> U+E755
  ["f09f90bb", :undef], # [BEAR FACE] U+1F43B -> "[クマ]"
  ["f09f90b9", :undef], # [HAMSTER FACE] U+1F439 -> "[ハムスター]"
  ["f09f90ba", "ee9aa1"], # [WOLF FACE] U+1F43A -> U+E6A1
  ["f09f90ae", :undef], # [COW FACE] U+1F42E -> "[牛]"
  ["f09f90b0", :undef], # [RABBIT FACE] U+1F430 -> "[ウサギ]"
  ["f09f90b8", :undef], # [FROG FACE] U+1F438 -> "[カエル]"
  ["f09f90be", "ee9a98"], # [PAW PRINTS] U+1F43E -> U+E698
  ["f09f90b2", :undef], # [DRAGON FACE] U+1F432 -> "[辰]"
  ["f09f90bc", :undef], # [PANDA FACE] U+1F43C -> "[パンダ]"
  ["f09f90bd", "ee9d95"], # [PIG NOSE] U+1F43D -> U+E755
  ["f09f9880", "ee9bb1"], # [ANGRY FACE] U+1F600 -> U+E6F1
  ["f09f9881", "ee9bb3"], # [ANGUISHED FACE] U+1F601 -> U+E6F3
  ["f09f9882", "ee9bb4"], # [ASTONISHED FACE] U+1F602 -> U+E6F4
  ["f09f9883", "ee9bb2"], # [DISAPPOINTED FACE] U+1F603 -> U+E6F2
  ["f09f9884", "ee9bb4"], # [DIZZY FACE] U+1F604 -> U+E6F4
  ["f09f9885", "ee9ca3"], # [EXASPERATED FACE] U+1F605 -> U+E723
  ["f09f9886", "ee9ca5"], # [EXPRESSIONLESS FACE] U+1F606 -> U+E725
  ["f09f9887", "ee9ca6"], # [FACE WITH HEART-SHAPED EYES] U+1F607 -> U+E726
  ["f09f9888", "ee9d93"], # [FACE WITH LOOK OF TRIUMPH] U+1F608 -> U+E753
  ["f09f9889", "ee9ca8"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+1F609 -> U+E728
  ["f09f988a", "ee9ca8"], # [FACE WITH STUCK-OUT TONGUE] U+1F60A -> U+E728
  ["f09f988b", "ee9d92"], # [FACE SAVOURING DELICIOUS FOOD] U+1F60B -> U+E752
  ["f09f988c", "ee9ca6"], # [FACE THROWING A KISS] U+1F60C -> U+E726
  ["f09f988d", "ee9ca6"], # [FACE KISSING] U+1F60D -> U+E726
  ["f09f988e", :undef], # [FACE WITH MASK] U+1F60E -> "[風邪ひき]"
  ["f09f988f", "ee9caa"], # [FLUSHED FACE] U+1F60F -> U+E72A
  ["f09f9890", "ee9bb0"], # [HAPPY FACE WITH OPEN MOUTH] U+1F610 -> U+E6F0
  ["f09f9891", "ee9ca2"], # [HAPPY FACE WITH OPEN MOUTH AND COLD SWEAT] U+1F611 -> U+E722
  ["f09f9892", "ee9caa"], # [HAPPY FACE WITH OPEN MOUTH AND CLOSED EYES] U+1F612 -> U+E72A
  ["f09f9893", "ee9d93"], # [HAPPY FACE WITH GRIN] U+1F613 -> U+E753
  ["f09f9894", "ee9caa"], # [HAPPY AND CRYING FACE] U+1F614 -> U+E72A
  ["f09f9895", "ee9bb0"], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+1F615 -> U+E6F0
  ["e298ba", "ee9bb0"], # [WHITE SMILING FACE] U+263A -> U+E6F0
  ["f09f9896", "ee9bb0"], # [HAPPY FACE WITH OPEN MOUTH AND RAISED EYEBROWS] U+1F616 -> U+E6F0
  ["f09f9897", "ee9cae"], # [CRYING FACE] U+1F617 -> U+E72E
  ["f09f9898", "ee9cad"], # [LOUDLY CRYING FACE] U+1F618 -> U+E72D
  ["f09f9899", "ee9d97"], # [FEARFUL FACE] U+1F619 -> U+E757
  ["f09f989a", "ee9cab"], # [PERSEVERING FACE] U+1F61A -> U+E72B
  ["f09f989b", "ee9ca4"], # [POUTING FACE] U+1F61B -> U+E724
  ["f09f989c", "ee9ca1"], # [RELIEVED FACE] U+1F61C -> U+E721
  ["f09f989d", "ee9bb3"], # [CONFOUNDED FACE] U+1F61D -> U+E6F3
  ["f09f989e", "ee9ca0"], # [PENSIVE FACE] U+1F61E -> U+E720
  ["f09f989f", "ee9d97"], # [FACE SCREAMING IN FEAR] U+1F61F -> U+E757
  ["f09f98a0", "ee9c81"], # [SLEEPY FACE] U+1F620 -> U+E701
  ["f09f98a1", "ee9cac"], # [SMIRKING FACE] U+1F621 -> U+E72C
  ["f09f98a2", "ee9ca3"], # [FACE WITH COLD SWEAT] U+1F622 -> U+E723
  ["f09f98a3", "ee9ca3"], # [DISAPPOINTED BUT RELIEVED FACE] U+1F623 -> U+E723
  ["f09f98a4", "ee9cab"], # [TIRED FACE] U+1F624 -> U+E72B
  ["f09f98a5", "ee9ca9"], # [WINKING FACE] U+1F625 -> U+E729
  ["f09f98ab", "ee9bb0"], # [CAT FACE WITH OPEN MOUTH] U+1F62B -> U+E6F0
  ["f09f98ac", "ee9d93"], # [HAPPY CAT FACE WITH GRIN] U+1F62C -> U+E753
  ["f09f98ad", "ee9caa"], # [HAPPY AND CRYING CAT FACE] U+1F62D -> U+E72A
  ["f09f98ae", "ee9ca6"], # [CAT FACE KISSING] U+1F62E -> U+E726
  ["f09f98af", "ee9ca6"], # [CAT FACE WITH HEART-SHAPED EYES] U+1F62F -> U+E726
  ["f09f98b0", "ee9cae"], # [CRYING CAT FACE] U+1F630 -> U+E72E
  ["f09f98b1", "ee9ca4"], # [POUTING CAT FACE] U+1F631 -> U+E724
  ["f09f98b2", "ee9d93"], # [CAT FACE WITH TIGHTLY-CLOSED LIPS] U+1F632 -> U+E753
  ["f09f98b3", "ee9bb3"], # [ANGUISHED CAT FACE] U+1F633 -> U+E6F3
  ["f09f98b4", "ee9caf"], # [FACE WITH NO GOOD GESTURE] U+1F634 -> U+E72F
  ["f09f98b5", "ee9c8b"], # [FACE WITH OK GESTURE] U+1F635 -> U+E70B
  ["f09f98b6", :undef], # [PERSON BOWING DEEPLY] U+1F636 -> "m(_ _)m"
  ["f09f98b7", :undef], # [SEE-NO-EVIL MONKEY] U+1F637 -> "(/_＼)"
  ["f09f98b9", :undef], # [SPEAK-NO-EVIL MONKEY] U+1F639 -> "(・×・)"
  ["f09f98b8", :undef], # [HEAR-NO-EVIL MONKEY] U+1F638 -> "|(・×・)|"
  ["f09f98ba", :undef], # [PERSON RAISING ONE HAND] U+1F63A -> "(^-^)/"
  ["f09f98bb", :undef], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+1F63B -> "＼(^o^)／"
  ["f09f98bc", "ee9bb3"], # [PERSON FROWNING] U+1F63C -> U+E6F3
  ["f09f98bd", "ee9bb1"], # [PERSON WITH POUTING FACE] U+1F63D -> U+E6F1
  ["f09f98be", :undef], # [PERSON WITH FOLDED HANDS] U+1F63E -> "(&gt;人&lt;)"
  ["f09f8fa0", "ee99a3"], # [HOUSE BUILDING] U+1F3E0 -> U+E663
  ["f09f8fa1", "ee99a3"], # [HOUSE WITH GARDEN] U+1F3E1 -> U+E663
  ["f09f8fa2", "ee99a4"], # [OFFICE BUILDING] U+1F3E2 -> U+E664
  ["f09f8fa3", "ee99a5"], # [JAPANESE POST OFFICE] U+1F3E3 -> U+E665
  ["f09f8fa5", "ee99a6"], # [HOSPITAL] U+1F3E5 -> U+E666
  ["f09f8fa6", "ee99a7"], # [BANK] U+1F3E6 -> U+E667
  ["f09f8fa7", "ee99a8"], # [AUTOMATED TELLER MACHINE] U+1F3E7 -> U+E668
  ["f09f8fa8", "ee99a9"], # [HOTEL] U+1F3E8 -> U+E669
  ["f09f8fa9", "ee99a9ee9baf"], # [LOVE HOTEL] U+1F3E9 -> U+E669 U+E6EF
  ["f09f8faa", "ee99aa"], # [CONVENIENCE STORE] U+1F3EA -> U+E66A
  ["f09f8fab", "ee9cbe"], # [SCHOOL] U+1F3EB -> U+E73E
  ["e29baa", :undef], # [CHURCH] U+26EA -> "[教会]"
  ["e29bb2", :undef], # [FOUNTAIN] U+26F2 -> "[噴水]"
  ["f09f8fac", :undef], # [DEPARTMENT STORE] U+1F3EC -> "[デパート]"
  ["f09f8faf", :undef], # [JAPANESE CASTLE] U+1F3EF -> "[城]"
  ["f09f8fb0", :undef], # [EUROPEAN CASTLE] U+1F3F0 -> "[城]"
  ["f09f8fad", :undef], # [FACTORY] U+1F3ED -> "[工場]"
  ["e29a93", "ee99a1"], # [ANCHOR] U+2693 -> U+E661
  ["f09f8fae", "ee9d8b"], # [IZAKAYA LANTERN] U+1F3EE -> U+E74B
  ["f09f97bb", "ee9d80"], # [MOUNT FUJI] U+1F5FB -> U+E740
  ["f09f97bc", :undef], # [TOKYO TOWER] U+1F5FC -> "[東京タワー]"
  ["f09f97bd", :undef], # [STATUE OF LIBERTY] U+1F5FD -> "[自由の女神]"
  ["f09f97be", :undef], # [SILHOUETTE OF JAPAN] U+1F5FE -> "[日本地図]"
  ["f09f97bf", :undef], # [MOYAI] U+1F5FF -> "[モアイ]"
  ["f09f919e", "ee9a99"], # [MANS SHOE] U+1F45E -> U+E699
  ["f09f919f", "ee9a99"], # [ATHLETIC SHOE] U+1F45F -> U+E699
  ["f09f91a0", "ee99b4"], # [HIGH-HEELED SHOE] U+1F460 -> U+E674
  ["f09f91a1", "ee99b4"], # [WOMANS SANDAL] U+1F461 -> U+E674
  ["f09f91a2", :undef], # [WOMANS BOOTS] U+1F462 -> "[ブーツ]"
  ["f09f91a3", "ee9a98"], # [FOOTPRINTS] U+1F463 -> U+E698
  ["f09f9193", "ee9a9a"], # [EYEGLASSES] U+1F453 -> U+E69A
  ["f09f9195", "ee9c8e"], # [T-SHIRT] U+1F455 -> U+E70E
  ["f09f9196", "ee9c91"], # [JEANS] U+1F456 -> U+E711
  ["f09f9191", "ee9c9a"], # [CROWN] U+1F451 -> U+E71A
  ["f09f9194", :undef], # [NECKTIE] U+1F454 -> "[ネクタイ]"
  ["f09f9192", :undef], # [WOMANS HAT] U+1F452 -> "[帽子]"
  ["f09f9197", :undef], # [DRESS] U+1F457 -> "[ドレス]"
  ["f09f9198", :undef], # [KIMONO] U+1F458 -> "[着物]"
  ["f09f9199", :undef], # [BIKINI] U+1F459 -> "[ビキニ]"
  ["f09f919a", "ee9c8e"], # [WOMANS CLOTHES] U+1F45A -> U+E70E
  ["f09f919b", "ee9c8f"], # [PURSE] U+1F45B -> U+E70F
  ["f09f919c", "ee9a82"], # [HANDBAG] U+1F45C -> U+E682
  ["f09f919d", "ee9aad"], # [POUCH] U+1F45D -> U+E6AD
  ["f09f92b0", "ee9c95"], # [MONEY BAG] U+1F4B0 -> U+E715
  ["f09f92b1", :undef], # [CURRENCY EXCHANGE] U+1F4B1 -> "[$￥]"
  ["f09f92b9", :undef], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+1F4B9 -> "[株価]"
  ["f09f92b2", "ee9c95"], # [HEAVY DOLLAR SIGN] U+1F4B2 -> U+E715
  ["f09f92b3", :undef], # [CREDIT CARD] U+1F4B3 -> "[カード]"
  ["f09f92b4", "ee9b96"], # [BANKNOTE WITH YEN SIGN] U+1F4B4 -> U+E6D6
  ["f09f92b5", "ee9c95"], # [BANKNOTE WITH DOLLAR SIGN] U+1F4B5 -> U+E715
  ["f09f92b8", :undef], # [MONEY WITH WINGS] U+1F4B8 -> "[飛んでいくお金]"
  ["f09f87a6", :undef], # [REGIONAL INDICATOR SYMBOL LETTER A] U+1F1E6 -> "[A]"
  ["f09f87a7", :undef], # [REGIONAL INDICATOR SYMBOL LETTER B] U+1F1E7 -> "[B]"
  ["f09f87a8", :undef], # [REGIONAL INDICATOR SYMBOL LETTER C] U+1F1E8 -> "[C]"
  ["f09f87a9", :undef], # [REGIONAL INDICATOR SYMBOL LETTER D] U+1F1E9 -> "[D]"
  ["f09f87aa", :undef], # [REGIONAL INDICATOR SYMBOL LETTER E] U+1F1EA -> "[E]"
  ["f09f87ab", :undef], # [REGIONAL INDICATOR SYMBOL LETTER F] U+1F1EB -> "[F]"
  ["f09f87ac", :undef], # [REGIONAL INDICATOR SYMBOL LETTER G] U+1F1EC -> "[G]"
  ["f09f87ad", :undef], # [REGIONAL INDICATOR SYMBOL LETTER H] U+1F1ED -> "[H]"
  ["f09f87ae", :undef], # [REGIONAL INDICATOR SYMBOL LETTER I] U+1F1EE -> "[I]"
  ["f09f87af", :undef], # [REGIONAL INDICATOR SYMBOL LETTER J] U+1F1EF -> "[J]"
  ["f09f87b0", :undef], # [REGIONAL INDICATOR SYMBOL LETTER K] U+1F1F0 -> "[K]"
  ["f09f87b1", :undef], # [REGIONAL INDICATOR SYMBOL LETTER L] U+1F1F1 -> "[L]"
  ["f09f87b2", :undef], # [REGIONAL INDICATOR SYMBOL LETTER M] U+1F1F2 -> "[M]"
  ["f09f87b3", :undef], # [REGIONAL INDICATOR SYMBOL LETTER N] U+1F1F3 -> "[N]"
  ["f09f87b4", :undef], # [REGIONAL INDICATOR SYMBOL LETTER O] U+1F1F4 -> "[O]"
  ["f09f87b5", :undef], # [REGIONAL INDICATOR SYMBOL LETTER P] U+1F1F5 -> "[P]"
  ["f09f87b6", :undef], # [REGIONAL INDICATOR SYMBOL LETTER Q] U+1F1F6 -> "[Q]"
  ["f09f87b7", :undef], # [REGIONAL INDICATOR SYMBOL LETTER R] U+1F1F7 -> "[R]"
  ["f09f87b8", :undef], # [REGIONAL INDICATOR SYMBOL LETTER S] U+1F1F8 -> "[S]"
  ["f09f87b9", :undef], # [REGIONAL INDICATOR SYMBOL LETTER T] U+1F1F9 -> "[T]"
  ["f09f87ba", :undef], # [REGIONAL INDICATOR SYMBOL LETTER U] U+1F1FA -> "[U]"
  ["f09f87bb", :undef], # [REGIONAL INDICATOR SYMBOL LETTER V] U+1F1FB -> "[V]"
  ["f09f87bc", :undef], # [REGIONAL INDICATOR SYMBOL LETTER W] U+1F1FC -> "[W]"
  ["f09f87bd", :undef], # [REGIONAL INDICATOR SYMBOL LETTER X] U+1F1FD -> "[X]"
  ["f09f87be", :undef], # [REGIONAL INDICATOR SYMBOL LETTER Y] U+1F1FE -> "[Y]"
  ["f09f87bf", :undef], # [REGIONAL INDICATOR SYMBOL LETTER Z] U+1F1FF -> "[Z]"
  ["f09f94a5", :undef], # [FIRE] U+1F525 -> "[炎]"
  ["f09f94a6", "ee9bbb"], # [ELECTRIC TORCH] U+1F526 -> U+E6FB
  ["f09f94a7", "ee9c98"], # [WRENCH] U+1F527 -> U+E718
  ["f09f94a8", :undef], # [HAMMER] U+1F528 -> "[ハンマー]"
  ["f09f94a9", :undef], # [NUT AND BOLT] U+1F529 -> "[ネジ]"
  ["f09f94aa", :undef], # [HOCHO] U+1F52A -> "[包丁]"
  ["f09f94ab", :undef], # [PISTOL] U+1F52B -> "[ピストル]"
  ["f09f94ae", :undef], # [CRYSTAL BALL] U+1F52E -> "[占い]"
  ["f09f94af", :undef], # [SIX POINTED STAR WITH MIDDLE DOT] U+1F52F -> "[占い]"
  ["f09f94b0", :undef], # [JAPANESE SYMBOL FOR BEGINNER] U+1F530 -> "[若葉マーク]"
  ["f09f94b1", "ee9c9a"], # [TRIDENT EMBLEM] U+1F531 -> U+E71A
  ["f09f9289", :undef], # [SYRINGE] U+1F489 -> "[注射]"
  ["f09f928a", :undef], # [PILL] U+1F48A -> "[薬]"
  ["f09f85b0", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+1F170 -> "[A]"
  ["f09f85b1", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+1F171 -> "[B]"
  ["f09f868e", :undef], # [NEGATIVE SQUARED AB] U+1F18E -> "[AB]"
  ["f09f85be", :undef], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+1F17E -> "[O]"
  ["f09f8e80", "ee9a84"], # [RIBBON] U+1F380 -> U+E684
  ["f09f8e81", "ee9a85"], # [WRAPPED PRESENT] U+1F381 -> U+E685
  ["f09f8e82", "ee9a86"], # [BIRTHDAY CAKE] U+1F382 -> U+E686
  ["f09f8e84", "ee9aa4"], # [CHRISTMAS TREE] U+1F384 -> U+E6A4
  ["f09f8e85", :undef], # [FATHER CHRISTMAS] U+1F385 -> "[サンタ]"
  ["f09f8e8c", :undef], # [CROSSED FLAGS] U+1F38C -> "[祝日]"
  ["f09f8e86", :undef], # [FIREWORKS] U+1F386 -> "[花火]"
  ["f09f8e88", :undef], # [BALLOON] U+1F388 -> "[風船]"
  ["f09f8e89", :undef], # [PARTY POPPER] U+1F389 -> "[クラッカー]"
  ["f09f8e8d", :undef], # [PINE DECORATION] U+1F38D -> "[門松]"
  ["f09f8e8e", :undef], # [JAPANESE DOLLS] U+1F38E -> "[ひな祭り]"
  ["f09f8e93", :undef], # [GRADUATION CAP] U+1F393 -> "[卒業式]"
  ["f09f8e92", :undef], # [SCHOOL SATCHEL] U+1F392 -> "[ランドセル]"
  ["f09f8e8f", :undef], # [CARP STREAMER] U+1F38F -> "[こいのぼり]"
  ["f09f8e87", :undef], # [FIREWORK SPARKLER] U+1F387 -> "[線香花火]"
  ["f09f8e90", :undef], # [WIND CHIME] U+1F390 -> "[風鈴]"
  ["f09f8e83", :undef], # [JACK-O-LANTERN] U+1F383 -> "[ハロウィン]"
  ["f09f8e8a", :undef], # [CONFETTI BALL] U+1F38A -> "[オメデトウ]"
  ["f09f8e8b", :undef], # [TANABATA TREE] U+1F38B -> "[七夕]"
  ["f09f8e91", :undef], # [MOON VIEWING CEREMONY] U+1F391 -> "[お月見]"
  ["f09f939f", "ee999a"], # [PAGER] U+1F4DF -> U+E65A
  ["e2988e", "ee9a87"], # [BLACK TELEPHONE] U+260E -> U+E687
  ["f09f939e", "ee9a87"], # [TELEPHONE RECEIVER] U+1F4DE -> U+E687
  ["f09f93b1", "ee9a88"], # [MOBILE PHONE] U+1F4F1 -> U+E688
  ["f09f93b2", "ee9b8e"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+1F4F2 -> U+E6CE
  ["f09f939d", "ee9a89"], # [MEMO] U+1F4DD -> U+E689
  ["f09f93a0", "ee9b90"], # [FAX MACHINE] U+1F4E0 -> U+E6D0
  ["e29c89", "ee9b93"], # [ENVELOPE] U+2709 -> U+E6D3
  ["f09f93a8", "ee9b8f"], # [INCOMING ENVELOPE] U+1F4E8 -> U+E6CF
  ["f09f93a9", "ee9b8f"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+1F4E9 -> U+E6CF
  ["f09f93aa", "ee99a5"], # [CLOSED MAILBOX WITH LOWERED FLAG] U+1F4EA -> U+E665
  ["f09f93ab", "ee99a5"], # [CLOSED MAILBOX WITH RAISED FLAG] U+1F4EB -> U+E665
  ["f09f93ae", "ee99a5"], # [POSTBOX] U+1F4EE -> U+E665
  ["f09f93b0", :undef], # [NEWSPAPER] U+1F4F0 -> "[新聞]"
  ["f09f93a2", :undef], # [PUBLIC ADDRESS LOUDSPEAKER] U+1F4E2 -> "[スピーカ]"
  ["f09f93a3", :undef], # [CHEERING MEGAPHONE] U+1F4E3 -> "[メガホン]"
  ["f09f93a1", :undef], # [SATELLITE ANTENNA] U+1F4E1 -> "[アンテナ]"
  ["f09f93a4", :undef], # [OUTBOX TRAY] U+1F4E4 -> "[送信BOX]"
  ["f09f93a5", :undef], # [INBOX TRAY] U+1F4E5 -> "[受信BOX]"
  ["f09f93a6", "ee9a85"], # [PACKAGE] U+1F4E6 -> U+E685
  ["f09f93a7", "ee9b93"], # [E-MAIL SYMBOL] U+1F4E7 -> U+E6D3
  ["f09f94a0", :undef], # [INPUT SYMBOL FOR LATIN CAPITAL LETTERS] U+1F520 -> "[ABCD]"
  ["f09f94a1", :undef], # [INPUT SYMBOL FOR LATIN SMALL LETTERS] U+1F521 -> "[abcd]"
  ["f09f94a2", :undef], # [INPUT SYMBOL FOR NUMBERS] U+1F522 -> "[1234]"
  ["f09f94a3", :undef], # [INPUT SYMBOL FOR SYMBOLS] U+1F523 -> "[記号]"
  ["f09f94a4", :undef], # [INPUT SYMBOL FOR LATIN LETTERS] U+1F524 -> "[ABC]"
  ["e29c92", "ee9aae"], # [BLACK NIB] U+2712 -> U+E6AE
  ["f09f92ba", "ee9ab2"], # [SEAT] U+1F4BA -> U+E6B2
  ["f09f92bb", "ee9c96"], # [PERSONAL COMPUTER] U+1F4BB -> U+E716
  ["e29c8f", "ee9c99"], # [PENCIL] U+270F -> U+E719
  ["f09f938e", "ee9cb0"], # [PAPERCLIP] U+1F4CE -> U+E730
  ["f09f92bc", "ee9a82"], # [BRIEFCASE] U+1F4BC -> U+E682
  ["f09f92bd", :undef], # [MINIDISC] U+1F4BD -> "[MD]"
  ["f09f92be", :undef], # [FLOPPY DISK] U+1F4BE -> "[フロッピー]"
  ["f09f92bf", "ee9a8c"], # [OPTICAL DISC] U+1F4BF -> U+E68C
  ["f09f9380", "ee9a8c"], # [DVD] U+1F4C0 -> U+E68C
  ["e29c82", "ee99b5"], # [BLACK SCISSORS] U+2702 -> U+E675
  ["f09f938d", :undef], # [ROUND PUSHPIN] U+1F4CD -> "[画びょう]"
  ["f09f9383", "ee9a89"], # [PAGE WITH CURL] U+1F4C3 -> U+E689
  ["f09f9384", "ee9a89"], # [PAGE FACING UP] U+1F4C4 -> U+E689
  ["f09f9385", :undef], # [CALENDAR] U+1F4C5 -> "[カレンダー]"
  ["f09f9381", :undef], # [FILE FOLDER] U+1F4C1 -> "[フォルダ]"
  ["f09f9382", :undef], # [OPEN FILE FOLDER] U+1F4C2 -> "[フォルダ]"
  ["f09f9393", "ee9a83"], # [NOTEBOOK] U+1F4D3 -> U+E683
  ["f09f9396", "ee9a83"], # [OPEN BOOK] U+1F4D6 -> U+E683
  ["f09f9394", "ee9a83"], # [NOTEBOOK WITH DECORATIVE COVER] U+1F4D4 -> U+E683
  ["f09f9395", "ee9a83"], # [CLOSED BOOK] U+1F4D5 -> U+E683
  ["f09f9397", "ee9a83"], # [GREEN BOOK] U+1F4D7 -> U+E683
  ["f09f9398", "ee9a83"], # [BLUE BOOK] U+1F4D8 -> U+E683
  ["f09f9399", "ee9a83"], # [ORANGE BOOK] U+1F4D9 -> U+E683
  ["f09f939a", "ee9a83"], # [BOOKS] U+1F4DA -> U+E683
  ["f09f939b", :undef], # [NAME BADGE] U+1F4DB -> "[名札]"
  ["f09f939c", "ee9c8a"], # [SCROLL] U+1F4DC -> U+E70A
  ["f09f938b", "ee9a89"], # [CLIPBOARD] U+1F4CB -> U+E689
  ["f09f9386", :undef], # [TEAR-OFF CALENDAR] U+1F4C6 -> "[カレンダー]"
  ["f09f938a", :undef], # [BAR CHART] U+1F4CA -> "[グラフ]"
  ["f09f9388", :undef], # [CHART WITH UPWARDS TREND] U+1F4C8 -> "[グラフ]"
  ["f09f9389", :undef], # [CHART WITH DOWNWARDS TREND] U+1F4C9 -> "[グラフ]"
  ["f09f9387", "ee9a83"], # [CARD INDEX] U+1F4C7 -> U+E683
  ["f09f938c", :undef], # [PUSHPIN] U+1F4CC -> "[画びょう]"
  ["f09f9392", "ee9a83"], # [LEDGER] U+1F4D2 -> U+E683
  ["f09f938f", :undef], # [STRAIGHT RULER] U+1F4CF -> "[定規]"
  ["f09f9390", :undef], # [TRIANGULAR RULER] U+1F4D0 -> "[三角定規]"
  ["f09f9391", "ee9a89"], # [BOOKMARK TABS] U+1F4D1 -> U+E689
  ["f09f8ebd", "ee9992"], # [RUNNING SHIRT WITH SASH] U+1F3BD -> U+E652
  ["e29abe", "ee9993"], # [BASEBALL] U+26BE -> U+E653
  ["e29bb3", "ee9994"], # [FLAG IN HOLE] U+26F3 -> U+E654
  ["f09f8ebe", "ee9995"], # [TENNIS RACQUET AND BALL] U+1F3BE -> U+E655
  ["e29abd", "ee9996"], # [SOCCER BALL] U+26BD -> U+E656
  ["f09f8ebf", "ee9997"], # [SKI AND SKI BOOT] U+1F3BF -> U+E657
  ["f09f8f80", "ee9998"], # [BASKETBALL AND HOOP] U+1F3C0 -> U+E658
  ["f09f8f81", "ee9999"], # [CHEQUERED FLAG] U+1F3C1 -> U+E659
  ["f09f8f82", "ee9c92"], # [SNOWBOARDER] U+1F3C2 -> U+E712
  ["f09f8f83", "ee9cb3"], # [RUNNER] U+1F3C3 -> U+E733
  ["f09f8f84", "ee9c92"], # [SURFER] U+1F3C4 -> U+E712
  ["f09f8f86", :undef], # [TROPHY] U+1F3C6 -> "[トロフィー]"
  ["f09f8f88", :undef], # [AMERICAN FOOTBALL] U+1F3C8 -> "[フットボール]"
  ["f09f8f8a", :undef], # [SWIMMER] U+1F3CA -> "[水泳]"
  ["f09f9a86", "ee999b"], # [TRAIN] U+1F686 -> U+E65B
  ["f09f9a87", "ee999c"], # [METRO] U+1F687 -> U+E65C
  ["e29382", "ee999c"], # [CIRCLED LATIN CAPITAL LETTER M] U+24C2 -> U+E65C
  ["f09f9a84", "ee999d"], # [HIGH-SPEED TRAIN] U+1F684 -> U+E65D
  ["f09f9a85", "ee999d"], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+1F685 -> U+E65D
  ["f09f9a97", "ee999e"], # [AUTOMOBILE] U+1F697 -> U+E65E
  ["f09f9a99", "ee999f"], # [RECREATIONAL VEHICLE] U+1F699 -> U+E65F
  ["f09f9a8d", "ee99a0"], # [ONCOMING BUS] U+1F68D -> U+E660
  ["f09f9a8f", :undef], # [BUS STOP] U+1F68F -> "[バス停]"
  ["f09f9aa2", "ee99a1"], # [SHIP] U+1F6A2 -> U+E661
  ["e29c88", "ee99a2"], # [AIRPLANE] U+2708 -> U+E662
  ["e29bb5", "ee9aa3"], # [SAILBOAT] U+26F5 -> U+E6A3
  ["f09f9a89", :undef], # [STATION] U+1F689 -> "[駅]"
  ["f09f9a80", :undef], # [ROCKET] U+1F680 -> "[ロケット]"
  ["f09f9aa4", "ee9aa3"], # [SPEEDBOAT] U+1F6A4 -> U+E6A3
  ["f09f9a95", "ee999e"], # [TAXI] U+1F695 -> U+E65E
  ["f09f9a9a", :undef], # [DELIVERY TRUCK] U+1F69A -> "[トラック]"
  ["f09f9a92", :undef], # [FIRE ENGINE] U+1F692 -> "[消防車]"
  ["f09f9a91", :undef], # [AMBULANCE] U+1F691 -> "[救急車]"
  ["f09f9a93", :undef], # [POLICE CAR] U+1F693 -> "[パトカー]"
  ["e29bbd", "ee99ab"], # [FUEL PUMP] U+26FD -> U+E66B
  ["f09f85bf", "ee99ac"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+1F17F -> U+E66C
  ["f09f9aa5", "ee99ad"], # [HORIZONTAL TRAFFIC LIGHT] U+1F6A5 -> U+E66D
  ["f09f9aa7", :undef], # [CONSTRUCTION SIGN] U+1F6A7 -> "[工事中]"
  ["f09f9aa8", :undef], # [POLICE CARS REVOLVING LIGHT] U+1F6A8 -> "[パトカー]"
  ["e299a8", "ee9bb7"], # [HOT SPRINGS] U+2668 -> U+E6F7
  ["e29bba", :undef], # [TENT] U+26FA -> "[キャンプ]"
  ["f09f8ea0", "ee99b9"], # [CAROUSEL HORSE] U+1F3A0 -> U+E679
  ["f09f8ea1", :undef], # [FERRIS WHEEL] U+1F3A1 -> "[観覧車]"
  ["f09f8ea2", :undef], # [ROLLER COASTER] U+1F3A2 -> "[ジェットコースター]"
  ["f09f8ea3", "ee9d91"], # [FISHING POLE AND FISH] U+1F3A3 -> U+E751
  ["f09f8ea4", "ee99b6"], # [MICROPHONE] U+1F3A4 -> U+E676
  ["f09f8ea5", "ee99b7"], # [MOVIE CAMERA] U+1F3A5 -> U+E677
  ["f09f8ea6", "ee99b7"], # [CINEMA] U+1F3A6 -> U+E677
  ["f09f8ea7", "ee99ba"], # [HEADPHONE] U+1F3A7 -> U+E67A
  ["f09f8ea8", "ee99bb"], # [ARTIST PALETTE] U+1F3A8 -> U+E67B
  ["f09f8ea9", "ee99bc"], # [TOP HAT] U+1F3A9 -> U+E67C
  ["f09f8eaa", "ee99bd"], # [CIRCUS TENT] U+1F3AA -> U+E67D
  ["f09f8eab", "ee99be"], # [TICKET] U+1F3AB -> U+E67E
  ["f09f8eac", "ee9aac"], # [CLAPPER BOARD] U+1F3AC -> U+E6AC
  ["f09f8ead", :undef], # [PERFORMING ARTS] U+1F3AD -> "[演劇]"
  ["f09f8eae", "ee9a8b"], # [VIDEO GAME] U+1F3AE -> U+E68B
  ["f09f8084", :undef], # [MAHJONG TILE RED DRAGON] U+1F004 -> "[麻雀]"
  ["f09f8eaf", :undef], # [DIRECT HIT] U+1F3AF -> "[的中]"
  ["f09f8eb0", :undef], # [SLOT MACHINE] U+1F3B0 -> "[777]"
  ["f09f8eb1", :undef], # [BILLIARDS] U+1F3B1 -> "[ビリヤード]"
  ["f09f8eb2", :undef], # [GAME DIE] U+1F3B2 -> "[サイコロ]"
  ["f09f8eb3", :undef], # [BOWLING] U+1F3B3 -> "[ボーリング]"
  ["f09f8eb4", :undef], # [FLOWER PLAYING CARDS] U+1F3B4 -> "[花札]"
  ["f09f838f", :undef], # [PLAYING CARD BLACK JOKER] U+1F0CF -> "[ジョーカー]"
  ["f09f8eb5", "ee9bb6"], # [MUSICAL NOTE] U+1F3B5 -> U+E6F6
  ["f09f8eb6", "ee9bbf"], # [MULTIPLE MUSICAL NOTES] U+1F3B6 -> U+E6FF
  ["f09f8eb7", :undef], # [SAXOPHONE] U+1F3B7 -> "[サックス]"
  ["f09f8eb8", :undef], # [GUITAR] U+1F3B8 -> "[ギター]"
  ["f09f8eb9", :undef], # [MUSICAL KEYBOARD] U+1F3B9 -> "[ピアノ]"
  ["f09f8eba", :undef], # [TRUMPET] U+1F3BA -> "[トランペット]"
  ["f09f8ebb", :undef], # [VIOLIN] U+1F3BB -> "[バイオリン]"
  ["f09f8ebc", "ee9bbf"], # [MUSICAL SCORE] U+1F3BC -> U+E6FF
  ["e380bd", :undef], # [PART ALTERNATION MARK] U+303D -> "[歌記号]"
  ["f09f93b7", "ee9a81"], # [CAMERA] U+1F4F7 -> U+E681
  ["f09f93b9", "ee99b7"], # [VIDEO CAMERA] U+1F4F9 -> U+E677
  ["f09f93ba", "ee9a8a"], # [TELEVISION] U+1F4FA -> U+E68A
  ["f09f93bb", :undef], # [RADIO] U+1F4FB -> "[ラジオ]"
  ["f09f93bc", :undef], # [VIDEOCASSETTE] U+1F4FC -> "[ビデオ]"
  ["f09f928b", "ee9bb9"], # [KISS MARK] U+1F48B -> U+E6F9
  ["f09f928c", "ee9c97"], # [LOVE LETTER] U+1F48C -> U+E717
  ["f09f928d", "ee9c9b"], # [RING] U+1F48D -> U+E71B
  ["f09f928e", "ee9c9b"], # [GEM STONE] U+1F48E -> U+E71B
  ["f09f928f", "ee9bb9"], # [KISS] U+1F48F -> U+E6F9
  ["f09f9290", :undef], # [BOUQUET] U+1F490 -> "[花束]"
  ["f09f9291", "ee9bad"], # [COUPLE WITH HEART] U+1F491 -> U+E6ED
  ["f09f9292", :undef], # [WEDDING] U+1F492 -> "[結婚式]"
  ["f09f949e", :undef], # [NO ONE UNDER EIGHTEEN SYMBOL] U+1F51E -> "[18禁]"
  ["c2a9", "ee9cb1"], # [COPYRIGHT SIGN] U+A9 -> U+E731
  ["c2ae", "ee9cb6"], # [REGISTERED SIGN] U+AE -> U+E736
  ["e284a2", "ee9cb2"], # [TRADE MARK SIGN] U+2122 -> U+E732
  ["e284b9", :undef], # [INFORMATION SOURCE] U+2139 -> "[ｉ]"
  ["f09f949f", :undef], # [KEYCAP TEN] U+1F51F -> "[10]"
  ["f09f93b6", :undef], # [ANTENNA WITH BARS] U+1F4F6 -> "[バリ3]"
  ["f09f93b3", :undef], # [VIBRATION MODE] U+1F4F3 -> "[マナーモード]"
  ["f09f93b4", :undef], # [MOBILE PHONE OFF] U+1F4F4 -> "[ケータイOFF]"
  ["f09f8d94", "ee99b3"], # [HAMBURGER] U+1F354 -> U+E673
  ["f09f8d99", "ee9d89"], # [RICE BALL] U+1F359 -> U+E749
  ["f09f8db0", "ee9d8a"], # [SHORTCAKE] U+1F370 -> U+E74A
  ["f09f8d9c", "ee9d8c"], # [STEAMING BOWL] U+1F35C -> U+E74C
  ["f09f8d9e", "ee9d8d"], # [BREAD] U+1F35E -> U+E74D
  ["f09f8db3", :undef], # [COOKING] U+1F373 -> "[フライパン]"
  ["f09f8da6", :undef], # [SOFT ICE CREAM] U+1F366 -> "[ソフトクリーム]"
  ["f09f8d9f", :undef], # [FRENCH FRIES] U+1F35F -> "[ポテト]"
  ["f09f8da1", :undef], # [DANGO] U+1F361 -> "[だんご]"
  ["f09f8d98", :undef], # [RICE CRACKER] U+1F358 -> "[せんべい]"
  ["f09f8d9a", "ee9d8c"], # [COOKED RICE] U+1F35A -> U+E74C
  ["f09f8d9d", :undef], # [SPAGHETTI] U+1F35D -> "[パスタ]"
  ["f09f8d9b", :undef], # [CURRY AND RICE] U+1F35B -> "[カレー]"
  ["f09f8da2", :undef], # [ODEN] U+1F362 -> "[おでん]"
  ["f09f8da3", :undef], # [SUSHI] U+1F363 -> "[すし]"
  ["f09f8db1", :undef], # [BENTO BOX] U+1F371 -> "[弁当]"
  ["f09f8db2", :undef], # [POT OF FOOD] U+1F372 -> "[鍋]"
  ["f09f8da7", :undef], # [SHAVED ICE] U+1F367 -> "[カキ氷]"
  ["f09f8d96", :undef], # [MEAT ON BONE] U+1F356 -> "[肉]"
  ["f09f8da5", "ee9983"], # [FISH CAKE WITH SWIRL DESIGN] U+1F365 -> U+E643
  ["f09f8da0", :undef], # [ROASTED SWEET POTATO] U+1F360 -> "[やきいも]"
  ["f09f8d95", :undef], # [SLICE OF PIZZA] U+1F355 -> "[ピザ]"
  ["f09f8d97", :undef], # [POULTRY LEG] U+1F357 -> "[チキン]"
  ["f09f8da8", :undef], # [ICE CREAM] U+1F368 -> "[アイスクリーム]"
  ["f09f8da9", :undef], # [DOUGHNUT] U+1F369 -> "[ドーナツ]"
  ["f09f8daa", :undef], # [COOKIE] U+1F36A -> "[クッキー]"
  ["f09f8dab", :undef], # [CHOCOLATE BAR] U+1F36B -> "[チョコ]"
  ["f09f8dac", :undef], # [CANDY] U+1F36C -> "[キャンディ]"
  ["f09f8dad", :undef], # [LOLLIPOP] U+1F36D -> "[キャンディ]"
  ["f09f8dae", :undef], # [CUSTARD] U+1F36E -> "[プリン]"
  ["f09f8daf", :undef], # [HONEY POT] U+1F36F -> "[ハチミツ]"
  ["f09f8da4", :undef], # [FRIED SHRIMP] U+1F364 -> "[エビフライ]"
  ["f09f8db4", "ee99af"], # [FORK AND KNIFE] U+1F374 -> U+E66F
  ["e29895", "ee99b0"], # [HOT BEVERAGE] U+2615 -> U+E670
  ["f09f8db8", "ee99b1"], # [COCKTAIL GLASS] U+1F378 -> U+E671
  ["f09f8dba", "ee99b2"], # [BEER MUG] U+1F37A -> U+E672
  ["f09f8db5", "ee9c9e"], # [TEACUP WITHOUT HANDLE] U+1F375 -> U+E71E
  ["f09f8db6", "ee9d8b"], # [SAKE BOTTLE AND CUP] U+1F376 -> U+E74B
  ["f09f8db7", "ee9d96"], # [WINE GLASS] U+1F377 -> U+E756
  ["f09f8dbb", "ee99b2"], # [CLINKING BEER MUGS] U+1F37B -> U+E672
  ["f09f8db9", "ee99b1"], # [TROPICAL DRINK] U+1F379 -> U+E671
  ["e28697", "ee99b8"], # [NORTH EAST ARROW] U+2197 -> U+E678
  ["e28698", "ee9a96"], # [SOUTH EAST ARROW] U+2198 -> U+E696
  ["e28696", "ee9a97"], # [NORTH WEST ARROW] U+2196 -> U+E697
  ["e28699", "ee9aa5"], # [SOUTH WEST ARROW] U+2199 -> U+E6A5
  ["e2a4b4", "ee9bb5"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+2934 -> U+E6F5
  ["e2a4b5", "ee9c80"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+2935 -> U+E700
  ["e28694", "ee9cbc"], # [LEFT RIGHT ARROW] U+2194 -> U+E73C
  ["e28695", "ee9cbd"], # [UP DOWN ARROW] U+2195 -> U+E73D
  ["e2ac86", :undef], # [UPWARDS BLACK ARROW] U+2B06 -> "[↑]"
  ["e2ac87", :undef], # [DOWNWARDS BLACK ARROW] U+2B07 -> "[↓]"
  ["e29ea1", :undef], # [BLACK RIGHTWARDS ARROW] U+27A1 -> "[→]"
  ["e2ac85", :undef], # [LEFTWARDS BLACK ARROW] U+2B05 -> "[←]"
  ["e296b6", :undef], # [BLACK RIGHT-POINTING TRIANGLE] U+25B6 -> "[&gt;]"
  ["e29780", :undef], # [BLACK LEFT-POINTING TRIANGLE] U+25C0 -> "[&lt;]"
  ["e28fa9", :undef], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+23E9 -> "[&gt;&gt;]"
  ["e28faa", :undef], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+23EA -> "[&lt;&lt;]"
  ["e28fab", :undef], # [BLACK UP-POINTING DOUBLE TRIANGLE] U+23EB -> "▲"
  ["e28fac", :undef], # [BLACK DOWN-POINTING DOUBLE TRIANGLE] U+23EC -> "▼"
  ["f09f94ba", :undef], # [UP-POINTING RED TRIANGLE] U+1F53A -> "▲"
  ["f09f94bb", :undef], # [DOWN-POINTING RED TRIANGLE] U+1F53B -> "▼"
  ["f09f94bc", :undef], # [UP-POINTING SMALL RED TRIANGLE] U+1F53C -> "▲"
  ["f09f94bd", :undef], # [DOWN-POINTING SMALL RED TRIANGLE] U+1F53D -> "▼"
  ["e2ad95", "ee9aa0"], # [HEAVY LARGE CIRCLE] U+2B55 -> U+E6A0
  ["e29d8c", :undef], # [CROSS MARK] U+274C -> "[×]"
  ["e29d8e", :undef], # [NEGATIVE SQUARED CROSS MARK] U+274E -> "[×]"
  ["e29da2", "ee9c82"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+2762 -> U+E702
  ["e28189", "ee9c83"], # [EXCLAMATION QUESTION MARK] U+2049 -> U+E703
  ["e280bc", "ee9c84"], # [DOUBLE EXCLAMATION MARK] U+203C -> U+E704
  ["e29d93", :undef], # [BLACK QUESTION MARK ORNAMENT] U+2753 -> "[？]"
  ["e29d94", :undef], # [WHITE QUESTION MARK ORNAMENT] U+2754 -> "[？]"
  ["e29d95", "ee9c82"], # [WHITE EXCLAMATION MARK ORNAMENT] U+2755 -> U+E702
  ["e380b0", "ee9c89"], # [WAVY DASH] U+3030 -> U+E709
  ["e29eb0", "ee9c8a"], # [CURLY LOOP] U+27B0 -> U+E70A
  ["e29ebf", "ee9b9f"], # [DOUBLE CURLY LOOP] U+27BF -> U+E6DF
  ["e29da4", "ee9bac"], # [HEAVY BLACK HEART] U+2764 -> U+E6EC
  ["f09f9293", "ee9bad"], # [BEATING HEART] U+1F493 -> U+E6ED
  ["f09f9294", "ee9bae"], # [BROKEN HEART] U+1F494 -> U+E6EE
  ["f09f9295", "ee9baf"], # [TWO HEARTS] U+1F495 -> U+E6EF
  ["f09f9296", "ee9bac"], # [SPARKLING HEART] U+1F496 -> U+E6EC
  ["f09f9297", "ee9bad"], # [GROWING HEART] U+1F497 -> U+E6ED
  ["f09f9298", "ee9bac"], # [HEART WITH ARROW] U+1F498 -> U+E6EC
  ["f09f9299", "ee9bac"], # [BLUE HEART] U+1F499 -> U+E6EC
  ["f09f929a", "ee9bac"], # [GREEN HEART] U+1F49A -> U+E6EC
  ["f09f929b", "ee9bac"], # [YELLOW HEART] U+1F49B -> U+E6EC
  ["f09f929c", "ee9bac"], # [PURPLE HEART] U+1F49C -> U+E6EC
  ["f09f929d", "ee9bac"], # [HEART WITH RIBBON] U+1F49D -> U+E6EC
  ["f09f929e", "ee9bad"], # [REVOLVING HEARTS] U+1F49E -> U+E6ED
  ["f09f929f", "ee9bb8"], # [HEART DECORATION] U+1F49F -> U+E6F8
  ["e299a5", "ee9a8d"], # [BLACK HEART SUIT] U+2665 -> U+E68D
  ["e299a0", "ee9a8e"], # [BLACK SPADE SUIT] U+2660 -> U+E68E
  ["e299a6", "ee9a8f"], # [BLACK DIAMOND SUIT] U+2666 -> U+E68F
  ["e299a3", "ee9a90"], # [BLACK CLUB SUIT] U+2663 -> U+E690
  ["f09f9aac", "ee99bf"], # [SMOKING SYMBOL] U+1F6AC -> U+E67F
  ["f09f9aad", "ee9a80"], # [NO SMOKING SYMBOL] U+1F6AD -> U+E680
  ["e299bf", "ee9a9b"], # [WHEELCHAIR SYMBOL] U+267F -> U+E69B
  ["f09f9aa9", "ee9b9e"], # [TRIANGULAR FLAG ON POST] U+1F6A9 -> U+E6DE
  ["e29aa0", "ee9cb7"], # [WARNING SIGN] U+26A0 -> U+E737
  ["e29b94", "ee9caf"], # [NO ENTRY] U+26D4 -> U+E72F
  ["e299bb", "ee9cb5"], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+267B -> U+E735
  ["f09f9ab2", "ee9c9d"], # [BICYCLE] U+1F6B2 -> U+E71D
  ["f09f9ab6", "ee9cb3"], # [PEDESTRIAN] U+1F6B6 -> U+E733
  ["f09f9ab9", :undef], # [MENS SYMBOL] U+1F6B9 -> "[♂]"
  ["f09f9aba", :undef], # [WOMENS SYMBOL] U+1F6BA -> "[♀]"
  ["f09f9b80", "ee9bb7"], # [BATH] U+1F6C0 -> U+E6F7
  ["f09f9abb", "ee99ae"], # [RESTROOM] U+1F6BB -> U+E66E
  ["f09f9abd", "ee99ae"], # [TOILET] U+1F6BD -> U+E66E
  ["f09f9abe", "ee99ae"], # [WATER CLOSET] U+1F6BE -> U+E66E
  ["f09f9abc", :undef], # [BABY SYMBOL] U+1F6BC -> "[赤ちゃん]"
  ["f09f9aaa", "ee9c94"], # [DOOR] U+1F6AA -> U+E714
  ["f09f9aab", "ee9cb8"], # [NO ENTRY SIGN] U+1F6AB -> U+E738
  ["e29c94", :undef], # [HEAVY CHECK MARK] U+2714 -> "[チェックマーク]"
  ["f09f8691", "ee9b9b"], # [SQUARED CL] U+1F191 -> U+E6DB
  ["f09f8692", :undef], # [SQUARED COOL] U+1F192 -> "[COOL]"
  ["f09f8693", "ee9b97"], # [SQUARED FREE] U+1F193 -> U+E6D7
  ["f09f8694", "ee9b98"], # [SQUARED ID] U+1F194 -> U+E6D8
  ["f09f8695", "ee9b9d"], # [SQUARED NEW] U+1F195 -> U+E6DD
  ["f09f8696", "ee9caf"], # [SQUARED NG] U+1F196 -> U+E72F
  ["f09f8697", "ee9c8b"], # [SQUARED OK] U+1F197 -> U+E70B
  ["f09f8698", :undef], # [SQUARED SOS] U+1F198 -> "[SOS]"
  ["f09f8699", :undef], # [SQUARED UP WITH EXCLAMATION MARK] U+1F199 -> "[UP!]"
  ["f09f869a", :undef], # [SQUARED VS] U+1F19A -> "[VS]"
  ["f09f8881", :undef], # [SQUARED KATAKANA KOKO] U+1F201 -> "[ココ]"
  ["f09f8882", :undef], # [SQUARED KATAKANA SA] U+1F202 -> "[サービス]"
  ["f09f88b2", "ee9cb8"], # [SQUARED CJK UNIFIED IDEOGRAPH-7981] U+1F232 -> U+E738
  ["f09f88b3", "ee9cb9"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+1F233 -> U+E739
  ["f09f88b4", "ee9cba"], # [SQUARED CJK UNIFIED IDEOGRAPH-5408] U+1F234 -> U+E73A
  ["f09f88b5", "ee9cbb"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+1F235 -> U+E73B
  ["f09f88b6", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6709] U+1F236 -> "[有]"
  ["f09f889a", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7121] U+1F21A -> "[無]"
  ["f09f88b7", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6708] U+1F237 -> "[月]"
  ["f09f88b8", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7533] U+1F238 -> "[申]"
  ["f09f88b9", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+1F239 -> "[割]"
  ["f09f88af", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+1F22F -> "[指]"
  ["f09f88ba", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+1F23A -> "[営]"
  ["e38a99", "ee9cb4"], # [CIRCLED IDEOGRAPH SECRET] U+3299 -> U+E734
  ["e38a97", :undef], # [CIRCLED IDEOGRAPH CONGRATULATION] U+3297 -> "[祝]"
  ["f09f8990", :undef], # [CIRCLED IDEOGRAPH ADVANTAGE] U+1F250 -> "[得]"
  ["f09f8991", :undef], # [CIRCLED IDEOGRAPH ACCEPT] U+1F251 -> "[可]"
  ["e29e95", :undef], # [HEAVY PLUS SIGN] U+2795 -> "[＋]"
  ["e29e96", :undef], # [HEAVY MINUS SIGN] U+2796 -> "[－]"
  ["e29c96", :undef], # [HEAVY MULTIPLICATION X] U+2716 -> "[×]"
  ["e29e97", :undef], # [HEAVY DIVISION SIGN] U+2797 -> "[÷]"
  ["f09f92a0", "ee9bb8"], # [DIAMOND SHAPE WITH A DOT INSIDE] U+1F4A0 -> U+E6F8
  ["f09f92a1", "ee9bbb"], # [ELECTRIC LIGHT BULB] U+1F4A1 -> U+E6FB
  ["f09f92a2", "ee9bbc"], # [ANGER SYMBOL] U+1F4A2 -> U+E6FC
  ["f09f92a3", "ee9bbe"], # [BOMB] U+1F4A3 -> U+E6FE
  ["f09f92a4", "ee9c81"], # [SLEEPING SYMBOL] U+1F4A4 -> U+E701
  ["f09f92a5", "ee9c85"], # [COLLISION SYMBOL] U+1F4A5 -> U+E705
  ["f09f92a6", "ee9c86"], # [SPLASHING SWEAT SYMBOL] U+1F4A6 -> U+E706
  ["f09f92a7", "ee9c87"], # [DROP OF WATER] U+1F4A7 -> U+E707
  ["f09f92a8", "ee9c88"], # [DASH SYMBOL] U+1F4A8 -> U+E708
  ["f09f92a9", :undef], # [PILE OF POO] U+1F4A9 -> "[ウンチ]"
  ["f09f92aa", :undef], # [FLEXED BICEPS] U+1F4AA -> "[力こぶ]"
  ["f09f92ab", :undef], # [DIZZY SYMBOL] U+1F4AB -> "[クラクラ]"
  ["f09f92ac", :undef], # [SPEECH BALLOON] U+1F4AC -> "[フキダシ]"
  ["e29ca8", "ee9bba"], # [SPARKLES] U+2728 -> U+E6FA
  ["e29cb4", "ee9bb8"], # [EIGHT POINTED BLACK STAR] U+2734 -> U+E6F8
  ["e29cb3", "ee9bb8"], # [EIGHT SPOKED ASTERISK] U+2733 -> U+E6F8
  ["e29aaa", "ee9a9c"], # [MEDIUM WHITE CIRCLE] U+26AA -> U+E69C
  ["e29aab", "ee9a9c"], # [MEDIUM BLACK CIRCLE] U+26AB -> U+E69C
  ["f09f94b4", "ee9a9c"], # [LARGE RED CIRCLE] U+1F534 -> U+E69C
  ["f09f94b5", "ee9a9c"], # [LARGE BLUE CIRCLE] U+1F535 -> U+E69C
  ["f09f94b2", "ee9a9c"], # [BLACK SQUARE BUTTON] U+1F532 -> U+E69C
  ["f09f94b3", "ee9a9c"], # [WHITE SQUARE BUTTON] U+1F533 -> U+E69C
  ["e2ad90", :undef], # [WHITE MEDIUM STAR] U+2B50 -> "[☆]"
  ["e2ac9c", :undef], # [WHITE LARGE SQUARE] U+2B1C -> "■"
  ["e2ac9b", :undef], # [BLACK LARGE SQUARE] U+2B1B -> "■"
  ["e296ab", :undef], # [WHITE SMALL SQUARE] U+25AB -> "■"
  ["e296aa", :undef], # [BLACK SMALL SQUARE] U+25AA -> "■"
  ["e297bd", :undef], # [WHITE MEDIUM SMALL SQUARE] U+25FD -> "■"
  ["e297be", :undef], # [BLACK MEDIUM SMALL SQUARE] U+25FE -> "■"
  ["e297bb", :undef], # [WHITE MEDIUM SQUARE] U+25FB -> "■"
  ["e297bc", :undef], # [BLACK MEDIUM SQUARE] U+25FC -> "■"
  ["f09f94b6", :undef], # [LARGE ORANGE DIAMOND] U+1F536 -> "◆"
  ["f09f94b7", :undef], # [LARGE BLUE DIAMOND] U+1F537 -> "◆"
  ["f09f94b8", :undef], # [SMALL ORANGE DIAMOND] U+1F538 -> "◆"
  ["f09f94b9", :undef], # [SMALL BLUE DIAMOND] U+1F539 -> "◆"
  ["e29d87", "ee9bba"], # [SPARKLE] U+2747 -> U+E6FA
  ["f09f92ae", :undef], # [WHITE FLOWER] U+1F4AE -> "[花丸]"
  ["f09f92af", :undef], # [HUNDRED POINTS SYMBOL] U+1F4AF -> "[100点]"
  ["e286a9", "ee9b9a"], # [LEFTWARDS ARROW WITH HOOK] U+21A9 -> U+E6DA
  ["e286aa", :undef], # [RIGHTWARDS ARROW WITH HOOK] U+21AA -> "└→"
  ["f09f9483", "ee9cb5"], # [CLOCKWISE DOWNWARDS AND UPWARDS OPEN CIRCLE ARROWS] U+1F503 -> U+E735
  ["f09f948a", :undef], # [SPEAKER WITH THREE SOUND WAVES] U+1F50A -> "[スピーカ]"
  ["f09f948b", :undef], # [BATTERY] U+1F50B -> "[電池]"
  ["f09f948c", :undef], # [ELECTRIC PLUG] U+1F50C -> "[コンセント]"
  ["f09f948d", "ee9b9c"], # [LEFT-POINTING MAGNIFYING GLASS] U+1F50D -> U+E6DC
  ["f09f948e", "ee9b9c"], # [RIGHT-POINTING MAGNIFYING GLASS] U+1F50E -> U+E6DC
  ["f09f9492", "ee9b99"], # [LOCK] U+1F512 -> U+E6D9
  ["f09f9493", "ee9b99"], # [OPEN LOCK] U+1F513 -> U+E6D9
  ["f09f948f", "ee9b99"], # [LOCK WITH INK PEN] U+1F50F -> U+E6D9
  ["f09f9490", "ee9b99"], # [CLOSED LOCK WITH KEY] U+1F510 -> U+E6D9
  ["f09f9491", "ee9b99"], # [KEY] U+1F511 -> U+E6D9
  ["f09f9494", "ee9c93"], # [BELL] U+1F514 -> U+E713
  ["e29891", :undef], # [BALLOT BOX WITH CHECK] U+2611 -> "[チェックマーク]"
  ["f09f9498", :undef], # [RADIO BUTTON] U+1F518 -> "[ラジオボタン]"
  ["f09f9496", :undef], # [BOOKMARK] U+1F516 -> "[ブックマーク]"
  ["f09f9497", :undef], # [LINK SYMBOL] U+1F517 -> "[リンク]"
  ["f09f9499", :undef], # [BACK WITH LEFTWARDS ARROW ABOVE] U+1F519 -> "[←BACK]"
  ["f09f949a", "ee9ab9"], # [END WITH LEFTWARDS ARROW ABOVE] U+1F51A -> U+E6B9
  ["f09f949b", "ee9ab8"], # [ON WITH EXCLAMATION MARK WITH LEFT RIGHT ARROW ABOVE] U+1F51B -> U+E6B8
  ["f09f949c", "ee9ab7"], # [SOON WITH RIGHTWARDS ARROW ABOVE] U+1F51C -> U+E6B7
  ["f09f949d", :undef], # [TOP WITH UPWARDS ARROW ABOVE] U+1F51D -> "[TOP]"
  ["e28083", :undef], # [EM SPACE] U+2003 -> U+3013 (GETA)
  ["e28082", :undef], # [EN SPACE] U+2002 -> U+3013 (GETA)
  ["e28085", :undef], # [FOUR-PER-EM SPACE] U+2005 -> U+3013 (GETA)
  ["e29c85", :undef], # [WHITE HEAVY CHECK MARK] U+2705 -> "[チェックマーク]"
  ["e29c8a", "ee9a93"], # [RAISED FIST] U+270A -> U+E693
  ["e29c8b", "ee9a95"], # [RAISED HAND] U+270B -> U+E695
  ["e29c8c", "ee9a94"], # [VICTORY HAND] U+270C -> U+E694
  ["f09f918a", "ee9bbd"], # [FISTED HAND SIGN] U+1F44A -> U+E6FD
  ["f09f918d", "ee9ca7"], # [THUMBS UP SIGN] U+1F44D -> U+E727
  ["e2989d", :undef], # [WHITE UP POINTING INDEX] U+261D -> "[人差し指]"
  ["f09f9186", :undef], # [WHITE UP POINTING BACKHAND INDEX] U+1F446 -> "[↑]"
  ["f09f9187", :undef], # [WHITE DOWN POINTING BACKHAND INDEX] U+1F447 -> "[↓]"
  ["f09f9188", :undef], # [WHITE LEFT POINTING BACKHAND INDEX] U+1F448 -> "[←]"
  ["f09f9189", :undef], # [WHITE RIGHT POINTING BACKHAND INDEX] U+1F449 -> "[→]"
  ["f09f918b", "ee9a95"], # [WAVING HAND SIGN] U+1F44B -> U+E695
  ["f09f918f", :undef], # [CLAPPING HANDS SIGN] U+1F44F -> "[拍手]"
  ["f09f918c", "ee9c8b"], # [OK HAND SIGN] U+1F44C -> U+E70B
  ["f09f918e", "ee9c80"], # [THUMBS DOWN SIGN] U+1F44E -> U+E700
  ["f09f9190", "ee9a95"], # [OPEN HANDS SIGN] U+1F450 -> U+E695
]

EMOJI_EXCHANGE_TBL['UTF-8']['UTF8-KDDI'] = [
  ["e29880", "ee9288"], # [BLACK SUN WITH RAYS] U+2600 -> U+E488
  ["e29881", "ee928d"], # [CLOUD] U+2601 -> U+E48D
  ["e29894", "ee928c"], # [UMBRELLA WITH RAIN DROPS] U+2614 -> U+E48C
  ["e29b84", "ee9285"], # [SNOWMAN WITHOUT SNOW] U+26C4 -> U+E485
  ["e29aa1", "ee9287"], # [HIGH VOLTAGE SIGN] U+26A1 -> U+E487
  ["f09f8c80", "ee91a9"], # [CYCLONE] U+1F300 -> U+E469
  ["f09f8c81", "ee9698"], # [FOGGY] U+1F301 -> U+E598
  ["f09f8c82", "eeaba8"], # [CLOSED UMBRELLA] U+1F302 -> U+EAE8
  ["f09f8c83", "eeabb1"], # [NIGHT WITH STARS] U+1F303 -> U+EAF1
  ["f09f8c84", "eeabb4"], # [SUNRISE OVER MOUNTAINS] U+1F304 -> U+EAF4
  ["f09f8c85", "eeabb4"], # [SUNRISE] U+1F305 -> U+EAF4
  ["f09f8c87", "ee979a"], # [SUNSET OVER BUILDINGS] U+1F307 -> U+E5DA
  ["f09f8c88", "eeabb2"], # [RAINBOW] U+1F308 -> U+EAF2
  ["e29d84", "ee928a"], # [SNOWFLAKE] U+2744 -> U+E48A
  ["e29b85", "ee928e"], # [SUN BEHIND CLOUD] U+26C5 -> U+E48E
  ["f09f8c89", "ee92bf"], # [BRIDGE AT NIGHT] U+1F309 -> U+E4BF
  ["f09f8c8a", "eeadbc"], # [WATER WAVE] U+1F30A -> U+EB7C
  ["f09f8c8b", "eead93"], # [VOLCANO] U+1F30B -> U+EB53
  ["f09f8c8c", "eead9f"], # [MILKY WAY] U+1F30C -> U+EB5F
  ["f09f8c8f", "ee96b3"], # [EARTH GLOBE ASIA-AUSTRALIA] U+1F30F -> U+E5B3
  ["f09f8c91", "ee96a8"], # [NEW MOON SYMBOL] U+1F311 -> U+E5A8
  ["f09f8c94", "ee96a9"], # [WAXING GIBBOUS MOON SYMBOL] U+1F314 -> U+E5A9
  ["f09f8c93", "ee96aa"], # [FIRST QUARTER MOON SYMBOL] U+1F313 -> U+E5AA
  ["f09f8c99", "ee9286"], # [CRESCENT MOON] U+1F319 -> U+E486
  ["f09f8c95", :undef], # [FULL MOON SYMBOL] U+1F315 -> "○"
  ["f09f8c9b", "ee9289"], # [FIRST QUARTER MOON WITH FACE] U+1F31B -> U+E489
  ["f09f8c9f", "ee928b"], # [GLOWING STAR] U+1F31F -> U+E48B
  ["f09f8ca0", "ee91a8"], # [SHOOTING STAR] U+1F320 -> U+E468
  ["f09f9590", "ee9694"], # [CLOCK FACE ONE OCLOCK] U+1F550 -> U+E594
  ["f09f9591", "ee9694"], # [CLOCK FACE TWO OCLOCK] U+1F551 -> U+E594
  ["f09f9592", "ee9694"], # [CLOCK FACE THREE OCLOCK] U+1F552 -> U+E594
  ["f09f9593", "ee9694"], # [CLOCK FACE FOUR OCLOCK] U+1F553 -> U+E594
  ["f09f9594", "ee9694"], # [CLOCK FACE FIVE OCLOCK] U+1F554 -> U+E594
  ["f09f9595", "ee9694"], # [CLOCK FACE SIX OCLOCK] U+1F555 -> U+E594
  ["f09f9596", "ee9694"], # [CLOCK FACE SEVEN OCLOCK] U+1F556 -> U+E594
  ["f09f9597", "ee9694"], # [CLOCK FACE EIGHT OCLOCK] U+1F557 -> U+E594
  ["f09f9598", "ee9694"], # [CLOCK FACE NINE OCLOCK] U+1F558 -> U+E594
  ["f09f9599", "ee9694"], # [CLOCK FACE TEN OCLOCK] U+1F559 -> U+E594
  ["f09f959a", "ee9694"], # [CLOCK FACE ELEVEN OCLOCK] U+1F55A -> U+E594
  ["f09f959b", "ee9694"], # [CLOCK FACE TWELVE OCLOCK] U+1F55B -> U+E594
  ["e28c9a", "ee95ba"], # [WATCH] U+231A -> U+E57A
  ["e28c9b", "ee95bb"], # [HOURGLASS] U+231B -> U+E57B
  ["e28fb0", "ee9694"], # [ALARM CLOCK] U+23F0 -> U+E594
  ["e28fb3", "ee91bc"], # [HOURGLASS WITH FLOWING SAND] U+23F3 -> U+E47C
  ["e29988", "ee928f"], # [ARIES] U+2648 -> U+E48F
  ["e29989", "ee9290"], # [TAURUS] U+2649 -> U+E490
  ["e2998a", "ee9291"], # [GEMINI] U+264A -> U+E491
  ["e2998b", "ee9292"], # [CANCER] U+264B -> U+E492
  ["e2998c", "ee9293"], # [LEO] U+264C -> U+E493
  ["e2998d", "ee9294"], # [VIRGO] U+264D -> U+E494
  ["e2998e", "ee9295"], # [LIBRA] U+264E -> U+E495
  ["e2998f", "ee9296"], # [SCORPIUS] U+264F -> U+E496
  ["e29990", "ee9297"], # [SAGITTARIUS] U+2650 -> U+E497
  ["e29991", "ee9298"], # [CAPRICORN] U+2651 -> U+E498
  ["e29992", "ee9299"], # [AQUARIUS] U+2652 -> U+E499
  ["e29993", "ee929a"], # [PISCES] U+2653 -> U+E49A
  ["e29b8e", "ee929b"], # [OPHIUCHUS] U+26CE -> U+E49B
  ["f09f8d80", "ee9493"], # [FOUR LEAF CLOVER] U+1F340 -> U+E513
  ["f09f8cb7", "ee93a4"], # [TULIP] U+1F337 -> U+E4E4
  ["f09f8cb1", "eeadbd"], # [SEEDLING] U+1F331 -> U+EB7D
  ["f09f8d81", "ee938e"], # [MAPLE LEAF] U+1F341 -> U+E4CE
  ["f09f8cb8", "ee938a"], # [CHERRY BLOSSOM] U+1F338 -> U+E4CA
  ["f09f8cb9", "ee96ba"], # [ROSE] U+1F339 -> U+E5BA
  ["f09f8d82", "ee978d"], # [FALLEN LEAF] U+1F342 -> U+E5CD
  ["f09f8d83", "ee978d"], # [LEAF FLUTTERING IN WIND] U+1F343 -> U+E5CD
  ["f09f8cba", "eeaa94"], # [HIBISCUS] U+1F33A -> U+EA94
  ["f09f8cbb", "ee93a3"], # [SUNFLOWER] U+1F33B -> U+E4E3
  ["f09f8cb4", "ee93a2"], # [PALM TREE] U+1F334 -> U+E4E2
  ["f09f8cb5", "eeaa96"], # [CACTUS] U+1F335 -> U+EA96
  ["f09f8cbe", :undef], # [EAR OF RICE] U+1F33E -> "[稲穂]"
  ["f09f8cbd", "eeacb6"], # [EAR OF MAIZE] U+1F33D -> U+EB36
  ["f09f8d84", "eeacb7"], # [MUSHROOM] U+1F344 -> U+EB37
  ["f09f8cb0", "eeacb8"], # [CHESTNUT] U+1F330 -> U+EB38
  ["f09f8cbc", "eead89"], # [BLOSSOM] U+1F33C -> U+EB49
  ["f09f8cbf", "eeae82"], # [HERB] U+1F33F -> U+EB82
  ["f09f8d92", "ee9392"], # [CHERRIES] U+1F352 -> U+E4D2
  ["f09f8d8c", "eeacb5"], # [BANANA] U+1F34C -> U+EB35
  ["f09f8d8e", "eeaab9"], # [RED APPLE] U+1F34E -> U+EAB9
  ["f09f8d8a", "eeaaba"], # [TANGERINE] U+1F34A -> U+EABA
  ["f09f8d93", "ee9394"], # [STRAWBERRY] U+1F353 -> U+E4D4
  ["f09f8d89", "ee938d"], # [WATERMELON] U+1F349 -> U+E4CD
  ["f09f8d85", "eeaabb"], # [TOMATO] U+1F345 -> U+EABB
  ["f09f8d86", "eeaabc"], # [AUBERGINE] U+1F346 -> U+EABC
  ["f09f8d88", "eeacb2"], # [MELON] U+1F348 -> U+EB32
  ["f09f8d8d", "eeacb3"], # [PINEAPPLE] U+1F34D -> U+EB33
  ["f09f8d87", "eeacb4"], # [GRAPES] U+1F347 -> U+EB34
  ["f09f8d91", "eeacb9"], # [PEACH] U+1F351 -> U+EB39
  ["f09f8d8f", "eead9a"], # [GREEN APPLE] U+1F34F -> U+EB5A
  ["f09f9180", "ee96a4"], # [EYES] U+1F440 -> U+E5A4
  ["f09f9182", "ee96a5"], # [EAR] U+1F442 -> U+E5A5
  ["f09f9183", "eeab90"], # [NOSE] U+1F443 -> U+EAD0
  ["f09f9184", "eeab91"], # [MOUTH] U+1F444 -> U+EAD1
  ["f09f9185", "eead87"], # [TONGUE] U+1F445 -> U+EB47
  ["f09f9284", "ee9489"], # [LIPSTICK] U+1F484 -> U+E509
  ["f09f9285", "eeaaa0"], # [NAIL POLISH] U+1F485 -> U+EAA0
  ["f09f9286", "ee948b"], # [FACE MASSAGE] U+1F486 -> U+E50B
  ["f09f9287", "eeaaa1"], # [HAIRCUT] U+1F487 -> U+EAA1
  ["f09f9288", "eeaaa2"], # [BARBER POLE] U+1F488 -> U+EAA2
  ["f09f91a4", :undef], # [BUST IN SILHOUETTE] U+1F464 -> U+3013 (GETA)
  ["f09f91a6", "ee93bc"], # [BOY] U+1F466 -> U+E4FC
  ["f09f91a7", "ee93ba"], # [GIRL] U+1F467 -> U+E4FA
  ["f09f91a8", "ee93bc"], # [MAN] U+1F468 -> U+E4FC
  ["f09f91a9", "ee93ba"], # [WOMAN] U+1F469 -> U+E4FA
  ["f09f91aa", "ee9481"], # [FAMILY] U+1F46A -> U+E501
  ["f09f91ab", :undef], # [MAN AND WOMAN HOLDING HANDS] U+1F46B -> "[カップル]"
  ["f09f91ae", "ee979d"], # [POLICE OFFICER] U+1F46E -> U+E5DD
  ["f09f91af", "eeab9b"], # [WOMAN WITH BUNNY EARS] U+1F46F -> U+EADB
  ["f09f91b0", "eeaba9"], # [BRIDE WITH VEIL] U+1F470 -> U+EAE9
  ["f09f91b1", "eeac93"], # [WESTERN PERSON] U+1F471 -> U+EB13
  ["f09f91b2", "eeac94"], # [MAN WITH GUA PI MAO] U+1F472 -> U+EB14
  ["f09f91b3", "eeac95"], # [MAN WITH TURBAN] U+1F473 -> U+EB15
  ["f09f91b4", "eeac96"], # [OLDER MAN] U+1F474 -> U+EB16
  ["f09f91b5", "eeac97"], # [OLDER WOMAN] U+1F475 -> U+EB17
  ["f09f91b6", "eeac98"], # [BABY] U+1F476 -> U+EB18
  ["f09f91b7", "eeac99"], # [CONSTRUCTION WORKER] U+1F477 -> U+EB19
  ["f09f91b8", "eeac9a"], # [PRINCESS] U+1F478 -> U+EB1A
  ["f09f91b9", "eead84"], # [JAPANESE OGRE] U+1F479 -> U+EB44
  ["f09f91ba", "eead85"], # [JAPANESE GOBLIN] U+1F47A -> U+EB45
  ["f09f91bb", "ee938b"], # [GHOST] U+1F47B -> U+E4CB
  ["f09f91bc", "ee96bf"], # [BABY ANGEL] U+1F47C -> U+E5BF
  ["f09f91bd", "ee948e"], # [EXTRATERRESTRIAL ALIEN] U+1F47D -> U+E50E
  ["f09f91be", "ee93ac"], # [ALIEN MONSTER] U+1F47E -> U+E4EC
  ["f09f91bf", "ee93af"], # [IMP] U+1F47F -> U+E4EF
  ["f09f9280", "ee93b8"], # [SKULL] U+1F480 -> U+E4F8
  ["f09f9281", :undef], # [INFORMATION DESK PERSON] U+1F481 -> "[案内]"
  ["f09f9282", :undef], # [GUARDSMAN] U+1F482 -> "[衛兵]"
  ["f09f9283", "eeac9c"], # [DANCER] U+1F483 -> U+EB1C
  ["f09f908c", "eeadbe"], # [SNAIL] U+1F40C -> U+EB7E
  ["f09f908d", "eeaca2"], # [SNAKE] U+1F40D -> U+EB22
  ["f09f908e", "ee9398"], # [HORSE] U+1F40E -> U+E4D8
  ["f09f9094", "eeaca3"], # [CHICKEN] U+1F414 -> U+EB23
  ["f09f9097", "eeaca4"], # [BOAR] U+1F417 -> U+EB24
  ["f09f90ab", "eeaca5"], # [BACTRIAN CAMEL] U+1F42B -> U+EB25
  ["f09f9098", "eeac9f"], # [ELEPHANT] U+1F418 -> U+EB1F
  ["f09f90a8", "eeaca0"], # [KOALA] U+1F428 -> U+EB20
  ["f09f9092", "ee9399"], # [MONKEY] U+1F412 -> U+E4D9
  ["f09f9091", "ee928f"], # [SHEEP] U+1F411 -> U+E48F
  ["f09f9099", "ee9787"], # [OCTOPUS] U+1F419 -> U+E5C7
  ["f09f909a", "eeabac"], # [SPIRAL SHELL] U+1F41A -> U+EAEC
  ["f09f909b", "eeac9e"], # [BUG] U+1F41B -> U+EB1E
  ["f09f909c", "ee939d"], # [ANT] U+1F41C -> U+E4DD
  ["f09f909d", "eead97"], # [HONEYBEE] U+1F41D -> U+EB57
  ["f09f909e", "eead98"], # [LADY BEETLE] U+1F41E -> U+EB58
  ["f09f90a0", "eeac9d"], # [TROPICAL FISH] U+1F420 -> U+EB1D
  ["f09f90a1", "ee9393"], # [BLOWFISH] U+1F421 -> U+E4D3
  ["f09f90a2", "ee9794"], # [TURTLE] U+1F422 -> U+E5D4
  ["f09f90a4", "ee93a0"], # [BABY CHICK] U+1F424 -> U+E4E0
  ["f09f90a5", "eeadb6"], # [FRONT-FACING BABY CHICK] U+1F425 -> U+EB76
  ["f09f90a6", "ee93a0"], # [BIRD] U+1F426 -> U+E4E0
  ["f09f90a3", "ee979b"], # [HATCHING CHICK] U+1F423 -> U+E5DB
  ["f09f90a7", "ee939c"], # [PENGUIN] U+1F427 -> U+E4DC
  ["f09f90a9", "ee939f"], # [POODLE] U+1F429 -> U+E4DF
  ["f09f909f", "ee929a"], # [FISH] U+1F41F -> U+E49A
  ["f09f90ac", "eeac9b"], # [DOLPHIN] U+1F42C -> U+EB1B
  ["f09f90ad", "ee9782"], # [MOUSE FACE] U+1F42D -> U+E5C2
  ["f09f90af", "ee9780"], # [TIGER FACE] U+1F42F -> U+E5C0
  ["f09f90b1", "ee939b"], # [CAT FACE] U+1F431 -> U+E4DB
  ["f09f90b3", "ee91b0"], # [SPOUTING WHALE] U+1F433 -> U+E470
  ["f09f90b4", "ee9398"], # [HORSE FACE] U+1F434 -> U+E4D8
  ["f09f90b5", "ee9399"], # [MONKEY FACE] U+1F435 -> U+E4D9
  ["f09f90b6", "ee93a1"], # [DOG FACE] U+1F436 -> U+E4E1
  ["f09f90b7", "ee939e"], # [PIG FACE] U+1F437 -> U+E4DE
  ["f09f90bb", "ee9781"], # [BEAR FACE] U+1F43B -> U+E5C1
  ["f09f90b9", :undef], # [HAMSTER FACE] U+1F439 -> "[ハムスター]"
  ["f09f90ba", "ee93a1"], # [WOLF FACE] U+1F43A -> U+E4E1
  ["f09f90ae", "eeaca1"], # [COW FACE] U+1F42E -> U+EB21
  ["f09f90b0", "ee9397"], # [RABBIT FACE] U+1F430 -> U+E4D7
  ["f09f90b8", "ee939a"], # [FROG FACE] U+1F438 -> U+E4DA
  ["f09f90be", "ee93ae"], # [PAW PRINTS] U+1F43E -> U+E4EE
  ["f09f90b2", "eeacbf"], # [DRAGON FACE] U+1F432 -> U+EB3F
  ["f09f90bc", "eead86"], # [PANDA FACE] U+1F43C -> U+EB46
  ["f09f90bd", "eead88"], # [PIG NOSE] U+1F43D -> U+EB48
  ["f09f9880", "ee91b2"], # [ANGRY FACE] U+1F600 -> U+E472
  ["f09f9881", "eeada7"], # [ANGUISHED FACE] U+1F601 -> U+EB67
  ["f09f9882", "eeab8a"], # [ASTONISHED FACE] U+1F602 -> U+EACA
  ["f09f9883", "eeab80"], # [DISAPPOINTED FACE] U+1F603 -> U+EAC0
  ["f09f9884", "ee96ae"], # [DIZZY FACE] U+1F604 -> U+E5AE
  ["f09f9885", "eeab8b"], # [EXASPERATED FACE] U+1F605 -> U+EACB
  ["f09f9886", "eeab89"], # [EXPRESSIONLESS FACE] U+1F606 -> U+EAC9
  ["f09f9887", "ee9784"], # [FACE WITH HEART-SHAPED EYES] U+1F607 -> U+E5C4
  ["f09f9888", "eeab81"], # [FACE WITH LOOK OF TRIUMPH] U+1F608 -> U+EAC1
  ["f09f9889", "ee93a7"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+1F609 -> U+E4E7
  ["f09f988a", "ee93a7"], # [FACE WITH STUCK-OUT TONGUE] U+1F60A -> U+E4E7
  ["f09f988b", "eeab8d"], # [FACE SAVOURING DELICIOUS FOOD] U+1F60B -> U+EACD
  ["f09f988c", "eeab8f"], # [FACE THROWING A KISS] U+1F60C -> U+EACF
  ["f09f988d", "eeab8e"], # [FACE KISSING] U+1F60D -> U+EACE
  ["f09f988e", "eeab87"], # [FACE WITH MASK] U+1F60E -> U+EAC7
  ["f09f988f", "eeab88"], # [FLUSHED FACE] U+1F60F -> U+EAC8
  ["f09f9890", "ee91b1"], # [HAPPY FACE WITH OPEN MOUTH] U+1F610 -> U+E471
  ["f09f9891", "ee91b1ee96b1"], # [HAPPY FACE WITH OPEN MOUTH AND COLD SWEAT] U+1F611 -> U+E471 U+E5B1
  ["f09f9892", "eeab85"], # [HAPPY FACE WITH OPEN MOUTH AND CLOSED EYES] U+1F612 -> U+EAC5
  ["f09f9893", "eeae80"], # [HAPPY FACE WITH GRIN] U+1F613 -> U+EB80
  ["f09f9894", "eeada4"], # [HAPPY AND CRYING FACE] U+1F614 -> U+EB64
  ["f09f9895", "eeab8d"], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+1F615 -> U+EACD
  ["e298ba", "ee93bb"], # [WHITE SMILING FACE] U+263A -> U+E4FB
  ["f09f9896", "ee91b1"], # [HAPPY FACE WITH OPEN MOUTH AND RAISED EYEBROWS] U+1F616 -> U+E471
  ["f09f9897", "eeada9"], # [CRYING FACE] U+1F617 -> U+EB69
  ["f09f9898", "ee91b3"], # [LOUDLY CRYING FACE] U+1F618 -> U+E473
  ["f09f9899", "eeab86"], # [FEARFUL FACE] U+1F619 -> U+EAC6
  ["f09f989a", "eeab82"], # [PERSEVERING FACE] U+1F61A -> U+EAC2
  ["f09f989b", "eead9d"], # [POUTING FACE] U+1F61B -> U+EB5D
  ["f09f989c", "eeab85"], # [RELIEVED FACE] U+1F61C -> U+EAC5
  ["f09f989d", "eeab83"], # [CONFOUNDED FACE] U+1F61D -> U+EAC3
  ["f09f989e", "eeab80"], # [PENSIVE FACE] U+1F61E -> U+EAC0
  ["f09f989f", "ee9785"], # [FACE SCREAMING IN FEAR] U+1F61F -> U+E5C5
  ["f09f98a0", "eeab84"], # [SLEEPY FACE] U+1F620 -> U+EAC4
  ["f09f98a1", "eeaabf"], # [SMIRKING FACE] U+1F621 -> U+EABF
  ["f09f98a2", "ee9786"], # [FACE WITH COLD SWEAT] U+1F622 -> U+E5C6
  ["f09f98a3", "ee9786"], # [DISAPPOINTED BUT RELIEVED FACE] U+1F623 -> U+E5C6
  ["f09f98a4", "ee91b4"], # [TIRED FACE] U+1F624 -> U+E474
  ["f09f98a5", "ee9783"], # [WINKING FACE] U+1F625 -> U+E5C3
  ["f09f98ab", "eeada1"], # [CAT FACE WITH OPEN MOUTH] U+1F62B -> U+EB61
  ["f09f98ac", "eeadbf"], # [HAPPY CAT FACE WITH GRIN] U+1F62C -> U+EB7F
  ["f09f98ad", "eeada3"], # [HAPPY AND CRYING CAT FACE] U+1F62D -> U+EB63
  ["f09f98ae", "eeada0"], # [CAT FACE KISSING] U+1F62E -> U+EB60
  ["f09f98af", "eeada5"], # [CAT FACE WITH HEART-SHAPED EYES] U+1F62F -> U+EB65
  ["f09f98b0", "eeada8"], # [CRYING CAT FACE] U+1F630 -> U+EB68
  ["f09f98b1", "eead9e"], # [POUTING CAT FACE] U+1F631 -> U+EB5E
  ["f09f98b2", "eeadaa"], # [CAT FACE WITH TIGHTLY-CLOSED LIPS] U+1F632 -> U+EB6A
  ["f09f98b3", "eeada6"], # [ANGUISHED CAT FACE] U+1F633 -> U+EB66
  ["f09f98b4", "eeab97"], # [FACE WITH NO GOOD GESTURE] U+1F634 -> U+EAD7
  ["f09f98b5", "eeab98"], # [FACE WITH OK GESTURE] U+1F635 -> U+EAD8
  ["f09f98b6", "eeab99"], # [PERSON BOWING DEEPLY] U+1F636 -> U+EAD9
  ["f09f98b7", "eead90"], # [SEE-NO-EVIL MONKEY] U+1F637 -> U+EB50
  ["f09f98b9", "eead91"], # [SPEAK-NO-EVIL MONKEY] U+1F639 -> U+EB51
  ["f09f98b8", "eead92"], # [HEAR-NO-EVIL MONKEY] U+1F638 -> U+EB52
  ["f09f98ba", "eeae85"], # [PERSON RAISING ONE HAND] U+1F63A -> U+EB85
  ["f09f98bb", "eeae86"], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+1F63B -> U+EB86
  ["f09f98bc", "eeae87"], # [PERSON FROWNING] U+1F63C -> U+EB87
  ["f09f98bd", "eeae88"], # [PERSON WITH POUTING FACE] U+1F63D -> U+EB88
  ["f09f98be", "eeab92"], # [PERSON WITH FOLDED HANDS] U+1F63E -> U+EAD2
  ["f09f8fa0", "ee92ab"], # [HOUSE BUILDING] U+1F3E0 -> U+E4AB
  ["f09f8fa1", "eeac89"], # [HOUSE WITH GARDEN] U+1F3E1 -> U+EB09
  ["f09f8fa2", "ee92ad"], # [OFFICE BUILDING] U+1F3E2 -> U+E4AD
  ["f09f8fa3", "ee979e"], # [JAPANESE POST OFFICE] U+1F3E3 -> U+E5DE
  ["f09f8fa5", "ee979f"], # [HOSPITAL] U+1F3E5 -> U+E5DF
  ["f09f8fa6", "ee92aa"], # [BANK] U+1F3E6 -> U+E4AA
  ["f09f8fa7", "ee92a3"], # [AUTOMATED TELLER MACHINE] U+1F3E7 -> U+E4A3
  ["f09f8fa8", "eeaa81"], # [HOTEL] U+1F3E8 -> U+EA81
  ["f09f8fa9", "eeabb3"], # [LOVE HOTEL] U+1F3E9 -> U+EAF3
  ["f09f8faa", "ee92a4"], # [CONVENIENCE STORE] U+1F3EA -> U+E4A4
  ["f09f8fab", "eeaa80"], # [SCHOOL] U+1F3EB -> U+EA80
  ["e29baa", "ee96bb"], # [CHURCH] U+26EA -> U+E5BB
  ["e29bb2", "ee978f"], # [FOUNTAIN] U+26F2 -> U+E5CF
  ["f09f8fac", "eeabb6"], # [DEPARTMENT STORE] U+1F3EC -> U+EAF6
  ["f09f8faf", "eeabb7"], # [JAPANESE CASTLE] U+1F3EF -> U+EAF7
  ["f09f8fb0", "eeabb8"], # [EUROPEAN CASTLE] U+1F3F0 -> U+EAF8
  ["f09f8fad", "eeabb9"], # [FACTORY] U+1F3ED -> U+EAF9
  ["e29a93", "ee92a9"], # [ANCHOR] U+2693 -> U+E4A9
  ["f09f8fae", "ee92bd"], # [IZAKAYA LANTERN] U+1F3EE -> U+E4BD
  ["f09f97bb", "ee96bd"], # [MOUNT FUJI] U+1F5FB -> U+E5BD
  ["f09f97bc", "ee9380"], # [TOKYO TOWER] U+1F5FC -> U+E4C0
  ["f09f97bd", :undef], # [STATUE OF LIBERTY] U+1F5FD -> "[自由の女神]"
  ["f09f97be", "ee95b2"], # [SILHOUETTE OF JAPAN] U+1F5FE -> U+E572
  ["f09f97bf", "eeadac"], # [MOYAI] U+1F5FF -> U+EB6C
  ["f09f919e", "ee96b7"], # [MANS SHOE] U+1F45E -> U+E5B7
  ["f09f919f", "eeacab"], # [ATHLETIC SHOE] U+1F45F -> U+EB2B
  ["f09f91a0", "ee949a"], # [HIGH-HEELED SHOE] U+1F460 -> U+E51A
  ["f09f91a1", "ee949a"], # [WOMANS SANDAL] U+1F461 -> U+E51A
  ["f09f91a2", "eeaa9f"], # [WOMANS BOOTS] U+1F462 -> U+EA9F
  ["f09f91a3", "eeacaa"], # [FOOTPRINTS] U+1F463 -> U+EB2A
  ["f09f9193", "ee93be"], # [EYEGLASSES] U+1F453 -> U+E4FE
  ["f09f9195", "ee96b6"], # [T-SHIRT] U+1F455 -> U+E5B6
  ["f09f9196", "eeadb7"], # [JEANS] U+1F456 -> U+EB77
  ["f09f9191", "ee9789"], # [CROWN] U+1F451 -> U+E5C9
  ["f09f9194", "eeaa93"], # [NECKTIE] U+1F454 -> U+EA93
  ["f09f9192", "eeaa9e"], # [WOMANS HAT] U+1F452 -> U+EA9E
  ["f09f9197", "eeadab"], # [DRESS] U+1F457 -> U+EB6B
  ["f09f9198", "eeaaa3"], # [KIMONO] U+1F458 -> U+EAA3
  ["f09f9199", "eeaaa4"], # [BIKINI] U+1F459 -> U+EAA4
  ["f09f919a", "ee948d"], # [WOMANS CLOTHES] U+1F45A -> U+E50D
  ["f09f919b", "ee9484"], # [PURSE] U+1F45B -> U+E504
  ["f09f919c", "ee929c"], # [HANDBAG] U+1F45C -> U+E49C
  ["f09f919d", :undef], # [POUCH] U+1F45D -> "[ふくろ]"
  ["f09f92b0", "ee9387"], # [MONEY BAG] U+1F4B0 -> U+E4C7
  ["f09f92b1", :undef], # [CURRENCY EXCHANGE] U+1F4B1 -> "[$￥]"
  ["f09f92b9", "ee979c"], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+1F4B9 -> U+E5DC
  ["f09f92b2", "ee95b9"], # [HEAVY DOLLAR SIGN] U+1F4B2 -> U+E579
  ["f09f92b3", "ee95bc"], # [CREDIT CARD] U+1F4B3 -> U+E57C
  ["f09f92b4", "ee95bd"], # [BANKNOTE WITH YEN SIGN] U+1F4B4 -> U+E57D
  ["f09f92b5", "ee9685"], # [BANKNOTE WITH DOLLAR SIGN] U+1F4B5 -> U+E585
  ["f09f92b8", "eead9b"], # [MONEY WITH WINGS] U+1F4B8 -> U+EB5B
  ["f09f87a6", :undef], # [REGIONAL INDICATOR SYMBOL LETTER A] U+1F1E6 -> "[A]"
  ["f09f87a7", :undef], # [REGIONAL INDICATOR SYMBOL LETTER B] U+1F1E7 -> "[B]"
  ["f09f87a8", :undef], # [REGIONAL INDICATOR SYMBOL LETTER C] U+1F1E8 -> "[C]"
  ["f09f87a9", :undef], # [REGIONAL INDICATOR SYMBOL LETTER D] U+1F1E9 -> "[D]"
  ["f09f87aa", :undef], # [REGIONAL INDICATOR SYMBOL LETTER E] U+1F1EA -> "[E]"
  ["f09f87ab", :undef], # [REGIONAL INDICATOR SYMBOL LETTER F] U+1F1EB -> "[F]"
  ["f09f87ac", :undef], # [REGIONAL INDICATOR SYMBOL LETTER G] U+1F1EC -> "[G]"
  ["f09f87ad", :undef], # [REGIONAL INDICATOR SYMBOL LETTER H] U+1F1ED -> "[H]"
  ["f09f87ae", :undef], # [REGIONAL INDICATOR SYMBOL LETTER I] U+1F1EE -> "[I]"
  ["f09f87af", :undef], # [REGIONAL INDICATOR SYMBOL LETTER J] U+1F1EF -> "[J]"
  ["f09f87b0", :undef], # [REGIONAL INDICATOR SYMBOL LETTER K] U+1F1F0 -> "[K]"
  ["f09f87b1", :undef], # [REGIONAL INDICATOR SYMBOL LETTER L] U+1F1F1 -> "[L]"
  ["f09f87b2", :undef], # [REGIONAL INDICATOR SYMBOL LETTER M] U+1F1F2 -> "[M]"
  ["f09f87b3", :undef], # [REGIONAL INDICATOR SYMBOL LETTER N] U+1F1F3 -> "[N]"
  ["f09f87b4", :undef], # [REGIONAL INDICATOR SYMBOL LETTER O] U+1F1F4 -> "[O]"
  ["f09f87b5", :undef], # [REGIONAL INDICATOR SYMBOL LETTER P] U+1F1F5 -> "[P]"
  ["f09f87b6", :undef], # [REGIONAL INDICATOR SYMBOL LETTER Q] U+1F1F6 -> "[Q]"
  ["f09f87b7", :undef], # [REGIONAL INDICATOR SYMBOL LETTER R] U+1F1F7 -> "[R]"
  ["f09f87b8", :undef], # [REGIONAL INDICATOR SYMBOL LETTER S] U+1F1F8 -> "[S]"
  ["f09f87b9", :undef], # [REGIONAL INDICATOR SYMBOL LETTER T] U+1F1F9 -> "[T]"
  ["f09f87ba", :undef], # [REGIONAL INDICATOR SYMBOL LETTER U] U+1F1FA -> "[U]"
  ["f09f87bb", :undef], # [REGIONAL INDICATOR SYMBOL LETTER V] U+1F1FB -> "[V]"
  ["f09f87bc", :undef], # [REGIONAL INDICATOR SYMBOL LETTER W] U+1F1FC -> "[W]"
  ["f09f87bd", :undef], # [REGIONAL INDICATOR SYMBOL LETTER X] U+1F1FD -> "[X]"
  ["f09f87be", :undef], # [REGIONAL INDICATOR SYMBOL LETTER Y] U+1F1FE -> "[Y]"
  ["f09f87bf", :undef], # [REGIONAL INDICATOR SYMBOL LETTER Z] U+1F1FF -> "[Z]"
  ["f09f94a5", "ee91bb"], # [FIRE] U+1F525 -> U+E47B
  ["f09f94a6", "ee9683"], # [ELECTRIC TORCH] U+1F526 -> U+E583
  ["f09f94a7", "ee9687"], # [WRENCH] U+1F527 -> U+E587
  ["f09f94a8", "ee978b"], # [HAMMER] U+1F528 -> U+E5CB
  ["f09f94a9", "ee9681"], # [NUT AND BOLT] U+1F529 -> U+E581
  ["f09f94aa", "ee95bf"], # [HOCHO] U+1F52A -> U+E57F
  ["f09f94ab", "ee948a"], # [PISTOL] U+1F52B -> U+E50A
  ["f09f94ae", "eeaa8f"], # [CRYSTAL BALL] U+1F52E -> U+EA8F
  ["f09f94af", "eeaa8f"], # [SIX POINTED STAR WITH MIDDLE DOT] U+1F52F -> U+EA8F
  ["f09f94b0", "ee9280"], # [JAPANESE SYMBOL FOR BEGINNER] U+1F530 -> U+E480
  ["f09f94b1", "ee9789"], # [TRIDENT EMBLEM] U+1F531 -> U+E5C9
  ["f09f9289", "ee9490"], # [SYRINGE] U+1F489 -> U+E510
  ["f09f928a", "eeaa9a"], # [PILL] U+1F48A -> U+EA9A
  ["f09f85b0", "eeaca6"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+1F170 -> U+EB26
  ["f09f85b1", "eeaca7"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+1F171 -> U+EB27
  ["f09f868e", "eeaca9"], # [NEGATIVE SQUARED AB] U+1F18E -> U+EB29
  ["f09f85be", "eeaca8"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+1F17E -> U+EB28
  ["f09f8e80", "ee969f"], # [RIBBON] U+1F380 -> U+E59F
  ["f09f8e81", "ee938f"], # [WRAPPED PRESENT] U+1F381 -> U+E4CF
  ["f09f8e82", "ee96a0"], # [BIRTHDAY CAKE] U+1F382 -> U+E5A0
  ["f09f8e84", "ee9389"], # [CHRISTMAS TREE] U+1F384 -> U+E4C9
  ["f09f8e85", "eeabb0"], # [FATHER CHRISTMAS] U+1F385 -> U+EAF0
  ["f09f8e8c", "ee9799"], # [CROSSED FLAGS] U+1F38C -> U+E5D9
  ["f09f8e86", "ee978c"], # [FIREWORKS] U+1F386 -> U+E5CC
  ["f09f8e88", "eeaa9b"], # [BALLOON] U+1F388 -> U+EA9B
  ["f09f8e89", "eeaa9c"], # [PARTY POPPER] U+1F389 -> U+EA9C
  ["f09f8e8d", "eeaba3"], # [PINE DECORATION] U+1F38D -> U+EAE3
  ["f09f8e8e", "eeaba4"], # [JAPANESE DOLLS] U+1F38E -> U+EAE4
  ["f09f8e93", "eeaba5"], # [GRADUATION CAP] U+1F393 -> U+EAE5
  ["f09f8e92", "eeaba6"], # [SCHOOL SATCHEL] U+1F392 -> U+EAE6
  ["f09f8e8f", "eeaba7"], # [CARP STREAMER] U+1F38F -> U+EAE7
  ["f09f8e87", "eeabab"], # [FIREWORK SPARKLER] U+1F387 -> U+EAEB
  ["f09f8e90", "eeabad"], # [WIND CHIME] U+1F390 -> U+EAED
  ["f09f8e83", "eeabae"], # [JACK-O-LANTERN] U+1F383 -> U+EAEE
  ["f09f8e8a", "ee91af"], # [CONFETTI BALL] U+1F38A -> U+E46F
  ["f09f8e8b", "eeacbd"], # [TANABATA TREE] U+1F38B -> U+EB3D
  ["f09f8e91", "eeabaf"], # [MOON VIEWING CEREMONY] U+1F391 -> U+EAEF
  ["f09f939f", "ee969b"], # [PAGER] U+1F4DF -> U+E59B
  ["e2988e", "ee9696"], # [BLACK TELEPHONE] U+260E -> U+E596
  ["f09f939e", "ee949e"], # [TELEPHONE RECEIVER] U+1F4DE -> U+E51E
  ["f09f93b1", "ee9688"], # [MOBILE PHONE] U+1F4F1 -> U+E588
  ["f09f93b2", "eeac88"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+1F4F2 -> U+EB08
  ["f09f939d", "eeaa92"], # [MEMO] U+1F4DD -> U+EA92
  ["f09f93a0", "ee94a0"], # [FAX MACHINE] U+1F4E0 -> U+E520
  ["e29c89", "ee94a1"], # [ENVELOPE] U+2709 -> U+E521
  ["f09f93a8", "ee9691"], # [INCOMING ENVELOPE] U+1F4E8 -> U+E591
  ["f09f93a9", "eeada2"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+1F4E9 -> U+EB62
  ["f09f93aa", "ee949b"], # [CLOSED MAILBOX WITH LOWERED FLAG] U+1F4EA -> U+E51B
  ["f09f93ab", "eeac8a"], # [CLOSED MAILBOX WITH RAISED FLAG] U+1F4EB -> U+EB0A
  ["f09f93ae", "ee949b"], # [POSTBOX] U+1F4EE -> U+E51B
  ["f09f93b0", "ee968b"], # [NEWSPAPER] U+1F4F0 -> U+E58B
  ["f09f93a2", "ee9491"], # [PUBLIC ADDRESS LOUDSPEAKER] U+1F4E2 -> U+E511
  ["f09f93a3", "ee9491"], # [CHEERING MEGAPHONE] U+1F4E3 -> U+E511
  ["f09f93a1", "ee92a8"], # [SATELLITE ANTENNA] U+1F4E1 -> U+E4A8
  ["f09f93a4", "ee9692"], # [OUTBOX TRAY] U+1F4E4 -> U+E592
  ["f09f93a5", "ee9693"], # [INBOX TRAY] U+1F4E5 -> U+E593
  ["f09f93a6", "ee949f"], # [PACKAGE] U+1F4E6 -> U+E51F
  ["f09f93a7", "eeadb1"], # [E-MAIL SYMBOL] U+1F4E7 -> U+EB71
  ["f09f94a0", "eeabbd"], # [INPUT SYMBOL FOR LATIN CAPITAL LETTERS] U+1F520 -> U+EAFD
  ["f09f94a1", "eeabbe"], # [INPUT SYMBOL FOR LATIN SMALL LETTERS] U+1F521 -> U+EAFE
  ["f09f94a2", "eeabbf"], # [INPUT SYMBOL FOR NUMBERS] U+1F522 -> U+EAFF
  ["f09f94a3", "eeac80"], # [INPUT SYMBOL FOR SYMBOLS] U+1F523 -> U+EB00
  ["f09f94a4", "eead95"], # [INPUT SYMBOL FOR LATIN LETTERS] U+1F524 -> U+EB55
  ["e29c92", "eeac83"], # [BLACK NIB] U+2712 -> U+EB03
  ["f09f92ba", :undef], # [SEAT] U+1F4BA -> "[いす]"
  ["f09f92bb", "ee96b8"], # [PERSONAL COMPUTER] U+1F4BB -> U+E5B8
  ["e29c8f", "ee92a1"], # [PENCIL] U+270F -> U+E4A1
  ["f09f938e", "ee92a0"], # [PAPERCLIP] U+1F4CE -> U+E4A0
  ["f09f92bc", "ee978e"], # [BRIEFCASE] U+1F4BC -> U+E5CE
  ["f09f92bd", "ee9682"], # [MINIDISC] U+1F4BD -> U+E582
  ["f09f92be", "ee95a2"], # [FLOPPY DISK] U+1F4BE -> U+E562
  ["f09f92bf", "ee948c"], # [OPTICAL DISC] U+1F4BF -> U+E50C
  ["f09f9380", "ee948c"], # [DVD] U+1F4C0 -> U+E50C
  ["e29c82", "ee9496"], # [BLACK SCISSORS] U+2702 -> U+E516
  ["f09f938d", "ee95a0"], # [ROUND PUSHPIN] U+1F4CD -> U+E560
  ["f09f9383", "ee95a1"], # [PAGE WITH CURL] U+1F4C3 -> U+E561
  ["f09f9384", "ee95a9"], # [PAGE FACING UP] U+1F4C4 -> U+E569
  ["f09f9385", "ee95a3"], # [CALENDAR] U+1F4C5 -> U+E563
  ["f09f9381", "ee968f"], # [FILE FOLDER] U+1F4C1 -> U+E58F
  ["f09f9382", "ee9690"], # [OPEN FILE FOLDER] U+1F4C2 -> U+E590
  ["f09f9393", "ee95ab"], # [NOTEBOOK] U+1F4D3 -> U+E56B
  ["f09f9396", "ee929f"], # [OPEN BOOK] U+1F4D6 -> U+E49F
  ["f09f9394", "ee929d"], # [NOTEBOOK WITH DECORATIVE COVER] U+1F4D4 -> U+E49D
  ["f09f9395", "ee95a8"], # [CLOSED BOOK] U+1F4D5 -> U+E568
  ["f09f9397", "ee95a5"], # [GREEN BOOK] U+1F4D7 -> U+E565
  ["f09f9398", "ee95a6"], # [BLUE BOOK] U+1F4D8 -> U+E566
  ["f09f9399", "ee95a7"], # [ORANGE BOOK] U+1F4D9 -> U+E567
  ["f09f939a", "ee95af"], # [BOOKS] U+1F4DA -> U+E56F
  ["f09f939b", "ee949d"], # [NAME BADGE] U+1F4DB -> U+E51D
  ["f09f939c", "ee959f"], # [SCROLL] U+1F4DC -> U+E55F
  ["f09f938b", "ee95a4"], # [CLIPBOARD] U+1F4CB -> U+E564
  ["f09f9386", "ee95aa"], # [TEAR-OFF CALENDAR] U+1F4C6 -> U+E56A
  ["f09f938a", "ee95b4"], # [BAR CHART] U+1F4CA -> U+E574
  ["f09f9388", "ee95b5"], # [CHART WITH UPWARDS TREND] U+1F4C8 -> U+E575
  ["f09f9389", "ee95b6"], # [CHART WITH DOWNWARDS TREND] U+1F4C9 -> U+E576
  ["f09f9387", "ee95ac"], # [CARD INDEX] U+1F4C7 -> U+E56C
  ["f09f938c", "ee95ad"], # [PUSHPIN] U+1F4CC -> U+E56D
  ["f09f9392", "ee95ae"], # [LEDGER] U+1F4D2 -> U+E56E
  ["f09f938f", "ee95b0"], # [STRAIGHT RULER] U+1F4CF -> U+E570
  ["f09f9390", "ee92a2"], # [TRIANGULAR RULER] U+1F4D0 -> U+E4A2
  ["f09f9391", "eeac8b"], # [BOOKMARK TABS] U+1F4D1 -> U+EB0B
  ["f09f8ebd", :undef], # [RUNNING SHIRT WITH SASH] U+1F3BD -> U+3013 (GETA)
  ["e29abe", "ee92ba"], # [BASEBALL] U+26BE -> U+E4BA
  ["e29bb3", "ee9699"], # [FLAG IN HOLE] U+26F3 -> U+E599
  ["f09f8ebe", "ee92b7"], # [TENNIS RACQUET AND BALL] U+1F3BE -> U+E4B7
  ["e29abd", "ee92b6"], # [SOCCER BALL] U+26BD -> U+E4B6
  ["f09f8ebf", "eeaaac"], # [SKI AND SKI BOOT] U+1F3BF -> U+EAAC
  ["f09f8f80", "ee969a"], # [BASKETBALL AND HOOP] U+1F3C0 -> U+E59A
  ["f09f8f81", "ee92b9"], # [CHEQUERED FLAG] U+1F3C1 -> U+E4B9
  ["f09f8f82", "ee92b8"], # [SNOWBOARDER] U+1F3C2 -> U+E4B8
  ["f09f8f83", "ee91ab"], # [RUNNER] U+1F3C3 -> U+E46B
  ["f09f8f84", "eead81"], # [SURFER] U+1F3C4 -> U+EB41
  ["f09f8f86", "ee9793"], # [TROPHY] U+1F3C6 -> U+E5D3
  ["f09f8f88", "ee92bb"], # [AMERICAN FOOTBALL] U+1F3C8 -> U+E4BB
  ["f09f8f8a", "eeab9e"], # [SWIMMER] U+1F3CA -> U+EADE
  ["f09f9a86", "ee92b5"], # [TRAIN] U+1F686 -> U+E4B5
  ["f09f9a87", "ee96bc"], # [METRO] U+1F687 -> U+E5BC
  ["e29382", "ee96bc"], # [CIRCLED LATIN CAPITAL LETTER M] U+24C2 -> U+E5BC
  ["f09f9a84", "ee92b0"], # [HIGH-SPEED TRAIN] U+1F684 -> U+E4B0
  ["f09f9a85", "ee92b0"], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+1F685 -> U+E4B0
  ["f09f9a97", "ee92b1"], # [AUTOMOBILE] U+1F697 -> U+E4B1
  ["f09f9a99", "ee92b1"], # [RECREATIONAL VEHICLE] U+1F699 -> U+E4B1
  ["f09f9a8d", "ee92af"], # [ONCOMING BUS] U+1F68D -> U+E4AF
  ["f09f9a8f", "ee92a7"], # [BUS STOP] U+1F68F -> U+E4A7
  ["f09f9aa2", "eeaa82"], # [SHIP] U+1F6A2 -> U+EA82
  ["e29c88", "ee92b3"], # [AIRPLANE] U+2708 -> U+E4B3
  ["e29bb5", "ee92b4"], # [SAILBOAT] U+26F5 -> U+E4B4
  ["f09f9a89", "eeadad"], # [STATION] U+1F689 -> U+EB6D
  ["f09f9a80", "ee9788"], # [ROCKET] U+1F680 -> U+E5C8
  ["f09f9aa4", "ee92b4"], # [SPEEDBOAT] U+1F6A4 -> U+E4B4
  ["f09f9a95", "ee92b1"], # [TAXI] U+1F695 -> U+E4B1
  ["f09f9a9a", "ee92b2"], # [DELIVERY TRUCK] U+1F69A -> U+E4B2
  ["f09f9a92", "eeab9f"], # [FIRE ENGINE] U+1F692 -> U+EADF
  ["f09f9a91", "eeaba0"], # [AMBULANCE] U+1F691 -> U+EAE0
  ["f09f9a93", "eeaba1"], # [POLICE CAR] U+1F693 -> U+EAE1
  ["e29bbd", "ee95b1"], # [FUEL PUMP] U+26FD -> U+E571
  ["f09f85bf", "ee92a6"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+1F17F -> U+E4A6
  ["f09f9aa5", "ee91aa"], # [HORIZONTAL TRAFFIC LIGHT] U+1F6A5 -> U+E46A
  ["f09f9aa7", "ee9797"], # [CONSTRUCTION SIGN] U+1F6A7 -> U+E5D7
  ["f09f9aa8", "eeadb3"], # [POLICE CARS REVOLVING LIGHT] U+1F6A8 -> U+EB73
  ["e299a8", "ee92bc"], # [HOT SPRINGS] U+2668 -> U+E4BC
  ["e29bba", "ee9790"], # [TENT] U+26FA -> U+E5D0
  ["f09f8ea0", :undef], # [CAROUSEL HORSE] U+1F3A0 -> U+3013 (GETA)
  ["f09f8ea1", "ee91ad"], # [FERRIS WHEEL] U+1F3A1 -> U+E46D
  ["f09f8ea2", "eeaba2"], # [ROLLER COASTER] U+1F3A2 -> U+EAE2
  ["f09f8ea3", "eead82"], # [FISHING POLE AND FISH] U+1F3A3 -> U+EB42
  ["f09f8ea4", "ee9483"], # [MICROPHONE] U+1F3A4 -> U+E503
  ["f09f8ea5", "ee9497"], # [MOVIE CAMERA] U+1F3A5 -> U+E517
  ["f09f8ea6", "ee9497"], # [CINEMA] U+1F3A6 -> U+E517
  ["f09f8ea7", "ee9488"], # [HEADPHONE] U+1F3A7 -> U+E508
  ["f09f8ea8", "ee969c"], # [ARTIST PALETTE] U+1F3A8 -> U+E59C
  ["f09f8ea9", "eeabb5"], # [TOP HAT] U+1F3A9 -> U+EAF5
  ["f09f8eaa", "ee969e"], # [CIRCUS TENT] U+1F3AA -> U+E59E
  ["f09f8eab", "ee929e"], # [TICKET] U+1F3AB -> U+E49E
  ["f09f8eac", "ee92be"], # [CLAPPER BOARD] U+1F3AC -> U+E4BE
  ["f09f8ead", "ee969d"], # [PERFORMING ARTS] U+1F3AD -> U+E59D
  ["f09f8eae", "ee9386"], # [VIDEO GAME] U+1F3AE -> U+E4C6
  ["f09f8084", "ee9791"], # [MAHJONG TILE RED DRAGON] U+1F004 -> U+E5D1
  ["f09f8eaf", "ee9385"], # [DIRECT HIT] U+1F3AF -> U+E4C5
  ["f09f8eb0", "ee91ae"], # [SLOT MACHINE] U+1F3B0 -> U+E46E
  ["f09f8eb1", "eeab9d"], # [BILLIARDS] U+1F3B1 -> U+EADD
  ["f09f8eb2", "ee9388"], # [GAME DIE] U+1F3B2 -> U+E4C8
  ["f09f8eb3", "eead83"], # [BOWLING] U+1F3B3 -> U+EB43
  ["f09f8eb4", "eeadae"], # [FLOWER PLAYING CARDS] U+1F3B4 -> U+EB6E
  ["f09f838f", "eeadaf"], # [PLAYING CARD BLACK JOKER] U+1F0CF -> U+EB6F
  ["f09f8eb5", "ee96be"], # [MUSICAL NOTE] U+1F3B5 -> U+E5BE
  ["f09f8eb6", "ee9485"], # [MULTIPLE MUSICAL NOTES] U+1F3B6 -> U+E505
  ["f09f8eb7", :undef], # [SAXOPHONE] U+1F3B7 -> "[サックス]"
  ["f09f8eb8", "ee9486"], # [GUITAR] U+1F3B8 -> U+E506
  ["f09f8eb9", "eead80"], # [MUSICAL KEYBOARD] U+1F3B9 -> U+EB40
  ["f09f8eba", "eeab9c"], # [TRUMPET] U+1F3BA -> U+EADC
  ["f09f8ebb", "ee9487"], # [VIOLIN] U+1F3BB -> U+E507
  ["f09f8ebc", "eeab8c"], # [MUSICAL SCORE] U+1F3BC -> U+EACC
  ["e380bd", :undef], # [PART ALTERNATION MARK] U+303D -> "[歌記号]"
  ["f09f93b7", "ee9495"], # [CAMERA] U+1F4F7 -> U+E515
  ["f09f93b9", "ee95be"], # [VIDEO CAMERA] U+1F4F9 -> U+E57E
  ["f09f93ba", "ee9482"], # [TELEVISION] U+1F4FA -> U+E502
  ["f09f93bb", "ee96b9"], # [RADIO] U+1F4FB -> U+E5B9
  ["f09f93bc", "ee9680"], # [VIDEOCASSETTE] U+1F4FC -> U+E580
  ["f09f928b", "ee93ab"], # [KISS MARK] U+1F48B -> U+E4EB
  ["f09f928c", "eeadb8"], # [LOVE LETTER] U+1F48C -> U+EB78
  ["f09f928d", "ee9494"], # [RING] U+1F48D -> U+E514
  ["f09f928e", "ee9494"], # [GEM STONE] U+1F48E -> U+E514
  ["f09f928f", "ee978a"], # [KISS] U+1F48F -> U+E5CA
  ["f09f9290", "eeaa95"], # [BOUQUET] U+1F490 -> U+EA95
  ["f09f9291", "eeab9a"], # [COUPLE WITH HEART] U+1F491 -> U+EADA
  ["f09f9292", "ee96bb"], # [WEDDING] U+1F492 -> U+E5BB
  ["f09f949e", "eeaa83"], # [NO ONE UNDER EIGHTEEN SYMBOL] U+1F51E -> U+EA83
  ["c2a9", "ee9598"], # [COPYRIGHT SIGN] U+A9 -> U+E558
  ["c2ae", "ee9599"], # [REGISTERED SIGN] U+AE -> U+E559
  ["e284a2", "ee958e"], # [TRADE MARK SIGN] U+2122 -> U+E54E
  ["e284b9", "ee94b3"], # [INFORMATION SOURCE] U+2139 -> U+E533
  ["f09f949f", "ee94ab"], # [KEYCAP TEN] U+1F51F -> U+E52B
  ["f09f93b6", "eeaa84"], # [ANTENNA WITH BARS] U+1F4F6 -> U+EA84
  ["f09f93b3", "eeaa90"], # [VIBRATION MODE] U+1F4F3 -> U+EA90
  ["f09f93b4", "eeaa91"], # [MOBILE PHONE OFF] U+1F4F4 -> U+EA91
  ["f09f8d94", "ee9396"], # [HAMBURGER] U+1F354 -> U+E4D6
  ["f09f8d99", "ee9395"], # [RICE BALL] U+1F359 -> U+E4D5
  ["f09f8db0", "ee9390"], # [SHORTCAKE] U+1F370 -> U+E4D0
  ["f09f8d9c", "ee96b4"], # [STEAMING BOWL] U+1F35C -> U+E5B4
  ["f09f8d9e", "eeaaaf"], # [BREAD] U+1F35E -> U+EAAF
  ["f09f8db3", "ee9391"], # [COOKING] U+1F373 -> U+E4D1
  ["f09f8da6", "eeaab0"], # [SOFT ICE CREAM] U+1F366 -> U+EAB0
  ["f09f8d9f", "eeaab1"], # [FRENCH FRIES] U+1F35F -> U+EAB1
  ["f09f8da1", "eeaab2"], # [DANGO] U+1F361 -> U+EAB2
  ["f09f8d98", "eeaab3"], # [RICE CRACKER] U+1F358 -> U+EAB3
  ["f09f8d9a", "eeaab4"], # [COOKED RICE] U+1F35A -> U+EAB4
  ["f09f8d9d", "eeaab5"], # [SPAGHETTI] U+1F35D -> U+EAB5
  ["f09f8d9b", "eeaab6"], # [CURRY AND RICE] U+1F35B -> U+EAB6
  ["f09f8da2", "eeaab7"], # [ODEN] U+1F362 -> U+EAB7
  ["f09f8da3", "eeaab8"], # [SUSHI] U+1F363 -> U+EAB8
  ["f09f8db1", "eeaabd"], # [BENTO BOX] U+1F371 -> U+EABD
  ["f09f8db2", "eeaabe"], # [POT OF FOOD] U+1F372 -> U+EABE
  ["f09f8da7", "eeabaa"], # [SHAVED ICE] U+1F367 -> U+EAEA
  ["f09f8d96", "ee9384"], # [MEAT ON BONE] U+1F356 -> U+E4C4
  ["f09f8da5", "ee93ad"], # [FISH CAKE WITH SWIRL DESIGN] U+1F365 -> U+E4ED
  ["f09f8da0", "eeacba"], # [ROASTED SWEET POTATO] U+1F360 -> U+EB3A
  ["f09f8d95", "eeacbb"], # [SLICE OF PIZZA] U+1F355 -> U+EB3B
  ["f09f8d97", "eeacbc"], # [POULTRY LEG] U+1F357 -> U+EB3C
  ["f09f8da8", "eead8a"], # [ICE CREAM] U+1F368 -> U+EB4A
  ["f09f8da9", "eead8b"], # [DOUGHNUT] U+1F369 -> U+EB4B
  ["f09f8daa", "eead8c"], # [COOKIE] U+1F36A -> U+EB4C
  ["f09f8dab", "eead8d"], # [CHOCOLATE BAR] U+1F36B -> U+EB4D
  ["f09f8dac", "eead8e"], # [CANDY] U+1F36C -> U+EB4E
  ["f09f8dad", "eead8f"], # [LOLLIPOP] U+1F36D -> U+EB4F
  ["f09f8dae", "eead96"], # [CUSTARD] U+1F36E -> U+EB56
  ["f09f8daf", "eead99"], # [HONEY POT] U+1F36F -> U+EB59
  ["f09f8da4", "eeadb0"], # [FRIED SHRIMP] U+1F364 -> U+EB70
  ["f09f8db4", "ee92ac"], # [FORK AND KNIFE] U+1F374 -> U+E4AC
  ["e29895", "ee9697"], # [HOT BEVERAGE] U+2615 -> U+E597
  ["f09f8db8", "ee9382"], # [COCKTAIL GLASS] U+1F378 -> U+E4C2
  ["f09f8dba", "ee9383"], # [BEER MUG] U+1F37A -> U+E4C3
  ["f09f8db5", "eeaaae"], # [TEACUP WITHOUT HANDLE] U+1F375 -> U+EAAE
  ["f09f8db6", "eeaa97"], # [SAKE BOTTLE AND CUP] U+1F376 -> U+EA97
  ["f09f8db7", "ee9381"], # [WINE GLASS] U+1F377 -> U+E4C1
  ["f09f8dbb", "eeaa98"], # [CLINKING BEER MUGS] U+1F37B -> U+EA98
  ["f09f8db9", "eeacbe"], # [TROPICAL DRINK] U+1F379 -> U+EB3E
  ["e28697", "ee9595"], # [NORTH EAST ARROW] U+2197 -> U+E555
  ["e28698", "ee958d"], # [SOUTH EAST ARROW] U+2198 -> U+E54D
  ["e28696", "ee958c"], # [NORTH WEST ARROW] U+2196 -> U+E54C
  ["e28699", "ee9596"], # [SOUTH WEST ARROW] U+2199 -> U+E556
  ["e2a4b4", "eeacad"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+2934 -> U+EB2D
  ["e2a4b5", "eeacae"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+2935 -> U+EB2E
  ["e28694", "eeadba"], # [LEFT RIGHT ARROW] U+2194 -> U+EB7A
  ["e28695", "eeadbb"], # [UP DOWN ARROW] U+2195 -> U+EB7B
  ["e2ac86", "ee94bf"], # [UPWARDS BLACK ARROW] U+2B06 -> U+E53F
  ["e2ac87", "ee9580"], # [DOWNWARDS BLACK ARROW] U+2B07 -> U+E540
  ["e29ea1", "ee9592"], # [BLACK RIGHTWARDS ARROW] U+27A1 -> U+E552
  ["e2ac85", "ee9593"], # [LEFTWARDS BLACK ARROW] U+2B05 -> U+E553
  ["e296b6", "ee94ae"], # [BLACK RIGHT-POINTING TRIANGLE] U+25B6 -> U+E52E
  ["e29780", "ee94ad"], # [BLACK LEFT-POINTING TRIANGLE] U+25C0 -> U+E52D
  ["e28fa9", "ee94b0"], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+23E9 -> U+E530
  ["e28faa", "ee94af"], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+23EA -> U+E52F
  ["e28fab", "ee9585"], # [BLACK UP-POINTING DOUBLE TRIANGLE] U+23EB -> U+E545
  ["e28fac", "ee9584"], # [BLACK DOWN-POINTING DOUBLE TRIANGLE] U+23EC -> U+E544
  ["f09f94ba", "ee959a"], # [UP-POINTING RED TRIANGLE] U+1F53A -> U+E55A
  ["f09f94bb", "ee959b"], # [DOWN-POINTING RED TRIANGLE] U+1F53B -> U+E55B
  ["f09f94bc", "ee9583"], # [UP-POINTING SMALL RED TRIANGLE] U+1F53C -> U+E543
  ["f09f94bd", "ee9582"], # [DOWN-POINTING SMALL RED TRIANGLE] U+1F53D -> U+E542
  ["e2ad95", "eeaaad"], # [HEAVY LARGE CIRCLE] U+2B55 -> U+EAAD
  ["e29d8c", "ee9590"], # [CROSS MARK] U+274C -> U+E550
  ["e29d8e", "ee9591"], # [NEGATIVE SQUARED CROSS MARK] U+274E -> U+E551
  ["e29da2", "ee9282"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+2762 -> U+E482
  ["e28189", "eeacaf"], # [EXCLAMATION QUESTION MARK] U+2049 -> U+EB2F
  ["e280bc", "eeacb0"], # [DOUBLE EXCLAMATION MARK] U+203C -> U+EB30
  ["e29d93", "ee9283"], # [BLACK QUESTION MARK ORNAMENT] U+2753 -> U+E483
  ["e29d94", "ee9283"], # [WHITE QUESTION MARK ORNAMENT] U+2754 -> U+E483
  ["e29d95", "ee9282"], # [WHITE EXCLAMATION MARK ORNAMENT] U+2755 -> U+E482
  ["e380b0", :undef], # [WAVY DASH] U+3030 -> U+3013 (GETA)
  ["e29eb0", "eeacb1"], # [CURLY LOOP] U+27B0 -> U+EB31
  ["e29ebf", :undef], # [DOUBLE CURLY LOOP] U+27BF -> "[フリーダイヤル]"
  ["e29da4", "ee9695"], # [HEAVY BLACK HEART] U+2764 -> U+E595
  ["f09f9293", "eeadb5"], # [BEATING HEART] U+1F493 -> U+EB75
  ["f09f9294", "ee91b7"], # [BROKEN HEART] U+1F494 -> U+E477
  ["f09f9295", "ee91b8"], # [TWO HEARTS] U+1F495 -> U+E478
  ["f09f9296", "eeaaa6"], # [SPARKLING HEART] U+1F496 -> U+EAA6
  ["f09f9297", "eeadb5"], # [GROWING HEART] U+1F497 -> U+EB75
  ["f09f9298", "ee93aa"], # [HEART WITH ARROW] U+1F498 -> U+E4EA
  ["f09f9299", "eeaaa7"], # [BLUE HEART] U+1F499 -> U+EAA7
  ["f09f929a", "eeaaa8"], # [GREEN HEART] U+1F49A -> U+EAA8
  ["f09f929b", "eeaaa9"], # [YELLOW HEART] U+1F49B -> U+EAA9
  ["f09f929c", "eeaaaa"], # [PURPLE HEART] U+1F49C -> U+EAAA
  ["f09f929d", "eead94"], # [HEART WITH RIBBON] U+1F49D -> U+EB54
  ["f09f929e", "ee96af"], # [REVOLVING HEARTS] U+1F49E -> U+E5AF
  ["f09f929f", "ee9695"], # [HEART DECORATION] U+1F49F -> U+E595
  ["e299a5", "eeaaa5"], # [BLACK HEART SUIT] U+2665 -> U+EAA5
  ["e299a0", "ee96a1"], # [BLACK SPADE SUIT] U+2660 -> U+E5A1
  ["e299a6", "ee96a2"], # [BLACK DIAMOND SUIT] U+2666 -> U+E5A2
  ["e299a3", "ee96a3"], # [BLACK CLUB SUIT] U+2663 -> U+E5A3
  ["f09f9aac", "ee91bd"], # [SMOKING SYMBOL] U+1F6AC -> U+E47D
  ["f09f9aad", "ee91be"], # [NO SMOKING SYMBOL] U+1F6AD -> U+E47E
  ["e299bf", "ee91bf"], # [WHEELCHAIR SYMBOL] U+267F -> U+E47F
  ["f09f9aa9", "eeacac"], # [TRIANGULAR FLAG ON POST] U+1F6A9 -> U+EB2C
  ["e29aa0", "ee9281"], # [WARNING SIGN] U+26A0 -> U+E481
  ["e29b94", "ee9284"], # [NO ENTRY] U+26D4 -> U+E484
  ["e299bb", "eeadb9"], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+267B -> U+EB79
  ["f09f9ab2", "ee92ae"], # [BICYCLE] U+1F6B2 -> U+E4AE
  ["f09f9ab6", "eeadb2"], # [PEDESTRIAN] U+1F6B6 -> U+EB72
  ["f09f9ab9", :undef], # [MENS SYMBOL] U+1F6B9 -> "[♂]"
  ["f09f9aba", :undef], # [WOMENS SYMBOL] U+1F6BA -> "[♀]"
  ["f09f9b80", "ee9798"], # [BATH] U+1F6C0 -> U+E5D8
  ["f09f9abb", "ee92a5"], # [RESTROOM] U+1F6BB -> U+E4A5
  ["f09f9abd", "ee92a5"], # [TOILET] U+1F6BD -> U+E4A5
  ["f09f9abe", "ee92a5"], # [WATER CLOSET] U+1F6BE -> U+E4A5
  ["f09f9abc", "eeac98"], # [BABY SYMBOL] U+1F6BC -> U+EB18
  ["f09f9aaa", :undef], # [DOOR] U+1F6AA -> "[ドア]"
  ["f09f9aab", "ee9581"], # [NO ENTRY SIGN] U+1F6AB -> U+E541
  ["e29c94", "ee9597"], # [HEAVY CHECK MARK] U+2714 -> U+E557
  ["f09f8691", "ee96ab"], # [SQUARED CL] U+1F191 -> U+E5AB
  ["f09f8692", "eeaa85"], # [SQUARED COOL] U+1F192 -> U+EA85
  ["f09f8693", "ee95b8"], # [SQUARED FREE] U+1F193 -> U+E578
  ["f09f8694", "eeaa88"], # [SQUARED ID] U+1F194 -> U+EA88
  ["f09f8695", "ee96b5"], # [SQUARED NEW] U+1F195 -> U+E5B5
  ["f09f8696", :undef], # [SQUARED NG] U+1F196 -> "[NG]"
  ["f09f8697", "ee96ad"], # [SQUARED OK] U+1F197 -> U+E5AD
  ["f09f8698", "ee93a8"], # [SQUARED SOS] U+1F198 -> U+E4E8
  ["f09f8699", "ee948f"], # [SQUARED UP WITH EXCLAMATION MARK] U+1F199 -> U+E50F
  ["f09f869a", "ee9792"], # [SQUARED VS] U+1F19A -> U+E5D2
  ["f09f8881", :undef], # [SQUARED KATAKANA KOKO] U+1F201 -> "[ココ]"
  ["f09f8882", "eeaa87"], # [SQUARED KATAKANA SA] U+1F202 -> U+EA87
  ["f09f88b2", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7981] U+1F232 -> "[禁]"
  ["f09f88b3", "eeaa8a"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+1F233 -> U+EA8A
  ["f09f88b4", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5408] U+1F234 -> "[合]"
  ["f09f88b5", "eeaa89"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+1F235 -> U+EA89
  ["f09f88b6", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6709] U+1F236 -> "[有]"
  ["f09f889a", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7121] U+1F21A -> "[無]"
  ["f09f88b7", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-6708] U+1F237 -> "[月]"
  ["f09f88b8", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7533] U+1F238 -> "[申]"
  ["f09f88b9", "eeaa86"], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+1F239 -> U+EA86
  ["f09f88af", "eeaa8b"], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+1F22F -> U+EA8B
  ["f09f88ba", "eeaa8c"], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+1F23A -> U+EA8C
  ["e38a99", "ee93b1"], # [CIRCLED IDEOGRAPH SECRET] U+3299 -> U+E4F1
  ["e38a97", "eeaa99"], # [CIRCLED IDEOGRAPH CONGRATULATION] U+3297 -> U+EA99
  ["f09f8990", "ee93b7"], # [CIRCLED IDEOGRAPH ADVANTAGE] U+1F250 -> U+E4F7
  ["f09f8991", "eeac81"], # [CIRCLED IDEOGRAPH ACCEPT] U+1F251 -> U+EB01
  ["e29e95", "ee94bc"], # [HEAVY PLUS SIGN] U+2795 -> U+E53C
  ["e29e96", "ee94bd"], # [HEAVY MINUS SIGN] U+2796 -> U+E53D
  ["e29c96", "ee958f"], # [HEAVY MULTIPLICATION X] U+2716 -> U+E54F
  ["e29e97", "ee9594"], # [HEAVY DIVISION SIGN] U+2797 -> U+E554
  ["f09f92a0", :undef], # [DIAMOND SHAPE WITH A DOT INSIDE] U+1F4A0 -> U+3013 (GETA)
  ["f09f92a1", "ee91b6"], # [ELECTRIC LIGHT BULB] U+1F4A1 -> U+E476
  ["f09f92a2", "ee93a5"], # [ANGER SYMBOL] U+1F4A2 -> U+E4E5
  ["f09f92a3", "ee91ba"], # [BOMB] U+1F4A3 -> U+E47A
  ["f09f92a4", "ee91b5"], # [SLEEPING SYMBOL] U+1F4A4 -> U+E475
  ["f09f92a5", "ee96b0"], # [COLLISION SYMBOL] U+1F4A5 -> U+E5B0
  ["f09f92a6", "ee96b1"], # [SPLASHING SWEAT SYMBOL] U+1F4A6 -> U+E5B1
  ["f09f92a7", "ee93a6"], # [DROP OF WATER] U+1F4A7 -> U+E4E6
  ["f09f92a8", "ee93b4"], # [DASH SYMBOL] U+1F4A8 -> U+E4F4
  ["f09f92a9", "ee93b5"], # [PILE OF POO] U+1F4A9 -> U+E4F5
  ["f09f92aa", "ee93a9"], # [FLEXED BICEPS] U+1F4AA -> U+E4E9
  ["f09f92ab", "eead9c"], # [DIZZY SYMBOL] U+1F4AB -> U+EB5C
  ["f09f92ac", "ee93bd"], # [SPEECH BALLOON] U+1F4AC -> U+E4FD
  ["e29ca8", "eeaaab"], # [SPARKLES] U+2728 -> U+EAAB
  ["e29cb4", "ee91b9"], # [EIGHT POINTED BLACK STAR] U+2734 -> U+E479
  ["e29cb3", "ee94be"], # [EIGHT SPOKED ASTERISK] U+2733 -> U+E53E
  ["e29aaa", "ee94ba"], # [MEDIUM WHITE CIRCLE] U+26AA -> U+E53A
  ["e29aab", "ee94bb"], # [MEDIUM BLACK CIRCLE] U+26AB -> U+E53B
  ["f09f94b4", "ee958a"], # [LARGE RED CIRCLE] U+1F534 -> U+E54A
  ["f09f94b5", "ee958b"], # [LARGE BLUE CIRCLE] U+1F535 -> U+E54B
  ["f09f94b2", "ee958b"], # [BLACK SQUARE BUTTON] U+1F532 -> U+E54B
  ["f09f94b3", "ee958b"], # [WHITE SQUARE BUTTON] U+1F533 -> U+E54B
  ["e2ad90", "ee928b"], # [WHITE MEDIUM STAR] U+2B50 -> U+E48B
  ["e2ac9c", "ee9588"], # [WHITE LARGE SQUARE] U+2B1C -> U+E548
  ["e2ac9b", "ee9589"], # [BLACK LARGE SQUARE] U+2B1B -> U+E549
  ["e296ab", "ee94b1"], # [WHITE SMALL SQUARE] U+25AB -> U+E531
  ["e296aa", "ee94b2"], # [BLACK SMALL SQUARE] U+25AA -> U+E532
  ["e297bd", "ee94b4"], # [WHITE MEDIUM SMALL SQUARE] U+25FD -> U+E534
  ["e297be", "ee94b5"], # [BLACK MEDIUM SMALL SQUARE] U+25FE -> U+E535
  ["e297bb", "ee94b8"], # [WHITE MEDIUM SQUARE] U+25FB -> U+E538
  ["e297bc", "ee94b9"], # [BLACK MEDIUM SQUARE] U+25FC -> U+E539
  ["f09f94b6", "ee9586"], # [LARGE ORANGE DIAMOND] U+1F536 -> U+E546
  ["f09f94b7", "ee9587"], # [LARGE BLUE DIAMOND] U+1F537 -> U+E547
  ["f09f94b8", "ee94b6"], # [SMALL ORANGE DIAMOND] U+1F538 -> U+E536
  ["f09f94b9", "ee94b7"], # [SMALL BLUE DIAMOND] U+1F539 -> U+E537
  ["e29d87", "ee91ac"], # [SPARKLE] U+2747 -> U+E46C
  ["f09f92ae", "ee93b0"], # [WHITE FLOWER] U+1F4AE -> U+E4F0
  ["f09f92af", "ee93b2"], # [HUNDRED POINTS SYMBOL] U+1F4AF -> U+E4F2
  ["e286a9", "ee959d"], # [LEFTWARDS ARROW WITH HOOK] U+21A9 -> U+E55D
  ["e286aa", "ee959c"], # [RIGHTWARDS ARROW WITH HOOK] U+21AA -> U+E55C
  ["f09f9483", "eeac8d"], # [CLOCKWISE DOWNWARDS AND UPWARDS OPEN CIRCLE ARROWS] U+1F503 -> U+EB0D
  ["f09f948a", "ee9491"], # [SPEAKER WITH THREE SOUND WAVES] U+1F50A -> U+E511
  ["f09f948b", "ee9684"], # [BATTERY] U+1F50B -> U+E584
  ["f09f948c", "ee9689"], # [ELECTRIC PLUG] U+1F50C -> U+E589
  ["f09f948d", "ee9498"], # [LEFT-POINTING MAGNIFYING GLASS] U+1F50D -> U+E518
  ["f09f948e", "eeac85"], # [RIGHT-POINTING MAGNIFYING GLASS] U+1F50E -> U+EB05
  ["f09f9492", "ee949c"], # [LOCK] U+1F512 -> U+E51C
  ["f09f9493", "ee949c"], # [OPEN LOCK] U+1F513 -> U+E51C
  ["f09f948f", "eeac8c"], # [LOCK WITH INK PEN] U+1F50F -> U+EB0C
  ["f09f9490", "eeabbc"], # [CLOSED LOCK WITH KEY] U+1F510 -> U+EAFC
  ["f09f9491", "ee9499"], # [KEY] U+1F511 -> U+E519
  ["f09f9494", "ee9492"], # [BELL] U+1F514 -> U+E512
  ["e29891", "eeac82"], # [BALLOT BOX WITH CHECK] U+2611 -> U+EB02
  ["f09f9498", "eeac84"], # [RADIO BUTTON] U+1F518 -> U+EB04
  ["f09f9496", "eeac87"], # [BOOKMARK] U+1F516 -> U+EB07
  ["f09f9497", "ee968a"], # [LINK SYMBOL] U+1F517 -> U+E58A
  ["f09f9499", "eeac86"], # [BACK WITH LEFTWARDS ARROW ABOVE] U+1F519 -> U+EB06
  ["f09f949a", :undef], # [END WITH LEFTWARDS ARROW ABOVE] U+1F51A -> "[end]"
  ["f09f949b", :undef], # [ON WITH EXCLAMATION MARK WITH LEFT RIGHT ARROW ABOVE] U+1F51B -> "[ON]"
  ["f09f949c", :undef], # [SOON WITH RIGHTWARDS ARROW ABOVE] U+1F51C -> "[SOON]"
  ["f09f949d", :undef], # [TOP WITH UPWARDS ARROW ABOVE] U+1F51D -> "[TOP]"
  ["e28083", "ee968c"], # [EM SPACE] U+2003 -> U+E58C
  ["e28082", "ee968d"], # [EN SPACE] U+2002 -> U+E58D
  ["e28085", "ee968e"], # [FOUR-PER-EM SPACE] U+2005 -> U+E58E
  ["e29c85", "ee959e"], # [WHITE HEAVY CHECK MARK] U+2705 -> U+E55E
  ["e29c8a", "eeae83"], # [RAISED FIST] U+270A -> U+EB83
  ["e29c8b", "ee96a7"], # [RAISED HAND] U+270B -> U+E5A7
  ["e29c8c", "ee96a6"], # [VICTORY HAND] U+270C -> U+E5A6
  ["f09f918a", "ee93b3"], # [FISTED HAND SIGN] U+1F44A -> U+E4F3
  ["f09f918d", "ee93b9"], # [THUMBS UP SIGN] U+1F44D -> U+E4F9
  ["e2989d", "ee93b6"], # [WHITE UP POINTING INDEX] U+261D -> U+E4F6
  ["f09f9186", "eeaa8d"], # [WHITE UP POINTING BACKHAND INDEX] U+1F446 -> U+EA8D
  ["f09f9187", "eeaa8e"], # [WHITE DOWN POINTING BACKHAND INDEX] U+1F447 -> U+EA8E
  ["f09f9188", "ee93bf"], # [WHITE LEFT POINTING BACKHAND INDEX] U+1F448 -> U+E4FF
  ["f09f9189", "ee9480"], # [WHITE RIGHT POINTING BACKHAND INDEX] U+1F449 -> U+E500
  ["f09f918b", "eeab96"], # [WAVING HAND SIGN] U+1F44B -> U+EAD6
  ["f09f918f", "eeab93"], # [CLAPPING HANDS SIGN] U+1F44F -> U+EAD3
  ["f09f918c", "eeab94"], # [OK HAND SIGN] U+1F44C -> U+EAD4
  ["f09f918e", "eeab95"], # [THUMBS DOWN SIGN] U+1F44E -> U+EAD5
  ["f09f9190", "eeab96"], # [OPEN HANDS SIGN] U+1F450 -> U+EAD6
]

EMOJI_EXCHANGE_TBL['UTF-8']['UTF8-SoftBank'] = [
  ["e29880", "ee818a"], # [BLACK SUN WITH RAYS] U+2600 -> U+E04A
  ["e29881", "ee8189"], # [CLOUD] U+2601 -> U+E049
  ["e29894", "ee818b"], # [UMBRELLA WITH RAIN DROPS] U+2614 -> U+E04B
  ["e29b84", "ee8188"], # [SNOWMAN WITHOUT SNOW] U+26C4 -> U+E048
  ["e29aa1", "ee84bd"], # [HIGH VOLTAGE SIGN] U+26A1 -> U+E13D
  ["f09f8c80", "ee9183"], # [CYCLONE] U+1F300 -> U+E443
  ["f09f8c81", :undef], # [FOGGY] U+1F301 -> "[霧]"
  ["f09f8c82", "ee90bc"], # [CLOSED UMBRELLA] U+1F302 -> U+E43C
  ["f09f8c83", "ee918b"], # [NIGHT WITH STARS] U+1F303 -> U+E44B
  ["f09f8c84", "ee818d"], # [SUNRISE OVER MOUNTAINS] U+1F304 -> U+E04D
  ["f09f8c85", "ee9189"], # [SUNRISE] U+1F305 -> U+E449
  ["f09f8c87", "ee918a"], # [SUNSET OVER BUILDINGS] U+1F307 -> U+E44A
  ["f09f8c88", "ee918c"], # [RAINBOW] U+1F308 -> U+E44C
  ["e29d84", :undef], # [SNOWFLAKE] U+2744 -> "[雪結晶]"
  ["e29b85", "ee818aee8189"], # [SUN BEHIND CLOUD] U+26C5 -> U+E04A U+E049
  ["f09f8c89", "ee918b"], # [BRIDGE AT NIGHT] U+1F309 -> U+E44B
  ["f09f8c8a", "ee90be"], # [WATER WAVE] U+1F30A -> U+E43E
  ["f09f8c8b", :undef], # [VOLCANO] U+1F30B -> "[火山]"
  ["f09f8c8c", "ee918b"], # [MILKY WAY] U+1F30C -> U+E44B
  ["f09f8c8f", :undef], # [EARTH GLOBE ASIA-AUSTRALIA] U+1F30F -> "[地球]"
  ["f09f8c91", :undef], # [NEW MOON SYMBOL] U+1F311 -> "●"
  ["f09f8c94", "ee818c"], # [WAXING GIBBOUS MOON SYMBOL] U+1F314 -> U+E04C
  ["f09f8c93", "ee818c"], # [FIRST QUARTER MOON SYMBOL] U+1F313 -> U+E04C
  ["f09f8c99", "ee818c"], # [CRESCENT MOON] U+1F319 -> U+E04C
  ["f09f8c95", :undef], # [FULL MOON SYMBOL] U+1F315 -> "○"
  ["f09f8c9b", "ee818c"], # [FIRST QUARTER MOON WITH FACE] U+1F31B -> U+E04C
  ["f09f8c9f", "ee8cb5"], # [GLOWING STAR] U+1F31F -> U+E335
  ["f09f8ca0", :undef], # [SHOOTING STAR] U+1F320 -> "☆彡"
  ["f09f9590", "ee80a4"], # [CLOCK FACE ONE OCLOCK] U+1F550 -> U+E024
  ["f09f9591", "ee80a5"], # [CLOCK FACE TWO OCLOCK] U+1F551 -> U+E025
  ["f09f9592", "ee80a6"], # [CLOCK FACE THREE OCLOCK] U+1F552 -> U+E026
  ["f09f9593", "ee80a7"], # [CLOCK FACE FOUR OCLOCK] U+1F553 -> U+E027
  ["f09f9594", "ee80a8"], # [CLOCK FACE FIVE OCLOCK] U+1F554 -> U+E028
  ["f09f9595", "ee80a9"], # [CLOCK FACE SIX OCLOCK] U+1F555 -> U+E029
  ["f09f9596", "ee80aa"], # [CLOCK FACE SEVEN OCLOCK] U+1F556 -> U+E02A
  ["f09f9597", "ee80ab"], # [CLOCK FACE EIGHT OCLOCK] U+1F557 -> U+E02B
  ["f09f9598", "ee80ac"], # [CLOCK FACE NINE OCLOCK] U+1F558 -> U+E02C
  ["f09f9599", "ee80ad"], # [CLOCK FACE TEN OCLOCK] U+1F559 -> U+E02D
  ["f09f959a", "ee80ae"], # [CLOCK FACE ELEVEN OCLOCK] U+1F55A -> U+E02E
  ["f09f959b", "ee80af"], # [CLOCK FACE TWELVE OCLOCK] U+1F55B -> U+E02F
  ["e28c9a", :undef], # [WATCH] U+231A -> "[腕時計]"
  ["e28c9b", :undef], # [HOURGLASS] U+231B -> "[砂時計]"
  ["e28fb0", "ee80ad"], # [ALARM CLOCK] U+23F0 -> U+E02D
  ["e28fb3", :undef], # [HOURGLASS WITH FLOWING SAND] U+23F3 -> "[砂時計]"
  ["e29988", "ee88bf"], # [ARIES] U+2648 -> U+E23F
  ["e29989", "ee8980"], # [TAURUS] U+2649 -> U+E240
  ["e2998a", "ee8981"], # [GEMINI] U+264A -> U+E241
  ["e2998b", "ee8982"], # [CANCER] U+264B -> U+E242
  ["e2998c", "ee8983"], # [LEO] U+264C -> U+E243
  ["e2998d", "ee8984"], # [VIRGO] U+264D -> U+E244
  ["e2998e", "ee8985"], # [LIBRA] U+264E -> U+E245
  ["e2998f", "ee8986"], # [SCORPIUS] U+264F -> U+E246
  ["e29990", "ee8987"], # [SAGITTARIUS] U+2650 -> U+E247
  ["e29991", "ee8988"], # [CAPRICORN] U+2651 -> U+E248
  ["e29992", "ee8989"], # [AQUARIUS] U+2652 -> U+E249
  ["e29993", "ee898a"], # [PISCES] U+2653 -> U+E24A
  ["e29b8e", "ee898b"], # [OPHIUCHUS] U+26CE -> U+E24B
  ["f09f8d80", "ee8490"], # [FOUR LEAF CLOVER] U+1F340 -> U+E110
  ["f09f8cb7", "ee8c84"], # [TULIP] U+1F337 -> U+E304
  ["f09f8cb1", "ee8490"], # [SEEDLING] U+1F331 -> U+E110
  ["f09f8d81", "ee8498"], # [MAPLE LEAF] U+1F341 -> U+E118
  ["f09f8cb8", "ee80b0"], # [CHERRY BLOSSOM] U+1F338 -> U+E030
  ["f09f8cb9", "ee80b2"], # [ROSE] U+1F339 -> U+E032
  ["f09f8d82", "ee8499"], # [FALLEN LEAF] U+1F342 -> U+E119
  ["f09f8d83", "ee9187"], # [LEAF FLUTTERING IN WIND] U+1F343 -> U+E447
  ["f09f8cba", "ee8c83"], # [HIBISCUS] U+1F33A -> U+E303
  ["f09f8cbb", "ee8c85"], # [SUNFLOWER] U+1F33B -> U+E305
  ["f09f8cb4", "ee8c87"], # [PALM TREE] U+1F334 -> U+E307
  ["f09f8cb5", "ee8c88"], # [CACTUS] U+1F335 -> U+E308
  ["f09f8cbe", "ee9184"], # [EAR OF RICE] U+1F33E -> U+E444
  ["f09f8cbd", :undef], # [EAR OF MAIZE] U+1F33D -> "[とうもろこし]"
  ["f09f8d84", :undef], # [MUSHROOM] U+1F344 -> "[キノコ]"
  ["f09f8cb0", :undef], # [CHESTNUT] U+1F330 -> "[栗]"
  ["f09f8cbc", "ee8c85"], # [BLOSSOM] U+1F33C -> U+E305
  ["f09f8cbf", "ee8490"], # [HERB] U+1F33F -> U+E110
  ["f09f8d92", :undef], # [CHERRIES] U+1F352 -> "[さくらんぼ]"
  ["f09f8d8c", :undef], # [BANANA] U+1F34C -> "[バナナ]"
  ["f09f8d8e", "ee8d85"], # [RED APPLE] U+1F34E -> U+E345
  ["f09f8d8a", "ee8d86"], # [TANGERINE] U+1F34A -> U+E346
  ["f09f8d93", "ee8d87"], # [STRAWBERRY] U+1F353 -> U+E347
  ["f09f8d89", "ee8d88"], # [WATERMELON] U+1F349 -> U+E348
  ["f09f8d85", "ee8d89"], # [TOMATO] U+1F345 -> U+E349
  ["f09f8d86", "ee8d8a"], # [AUBERGINE] U+1F346 -> U+E34A
  ["f09f8d88", :undef], # [MELON] U+1F348 -> "[メロン]"
  ["f09f8d8d", :undef], # [PINEAPPLE] U+1F34D -> "[パイナップル]"
  ["f09f8d87", :undef], # [GRAPES] U+1F347 -> "[ブドウ]"
  ["f09f8d91", :undef], # [PEACH] U+1F351 -> "[モモ]"
  ["f09f8d8f", "ee8d85"], # [GREEN APPLE] U+1F34F -> U+E345
  ["f09f9180", "ee9099"], # [EYES] U+1F440 -> U+E419
  ["f09f9182", "ee909b"], # [EAR] U+1F442 -> U+E41B
  ["f09f9183", "ee909a"], # [NOSE] U+1F443 -> U+E41A
  ["f09f9184", "ee909c"], # [MOUTH] U+1F444 -> U+E41C
  ["f09f9185", "ee9089"], # [TONGUE] U+1F445 -> U+E409
  ["f09f9284", "ee8c9c"], # [LIPSTICK] U+1F484 -> U+E31C
  ["f09f9285", "ee8c9d"], # [NAIL POLISH] U+1F485 -> U+E31D
  ["f09f9286", "ee8c9e"], # [FACE MASSAGE] U+1F486 -> U+E31E
  ["f09f9287", "ee8c9f"], # [HAIRCUT] U+1F487 -> U+E31F
  ["f09f9288", "ee8ca0"], # [BARBER POLE] U+1F488 -> U+E320
  ["f09f91a4", :undef], # [BUST IN SILHOUETTE] U+1F464 -> U+3013 (GETA)
  ["f09f91a6", "ee8081"], # [BOY] U+1F466 -> U+E001
  ["f09f91a7", "ee8082"], # [GIRL] U+1F467 -> U+E002
  ["f09f91a8", "ee8084"], # [MAN] U+1F468 -> U+E004
  ["f09f91a9", "ee8085"], # [WOMAN] U+1F469 -> U+E005
  ["f09f91aa", :undef], # [FAMILY] U+1F46A -> "[家族]"
  ["f09f91ab", "ee90a8"], # [MAN AND WOMAN HOLDING HANDS] U+1F46B -> U+E428
  ["f09f91ae", "ee8592"], # [POLICE OFFICER] U+1F46E -> U+E152
  ["f09f91af", "ee90a9"], # [WOMAN WITH BUNNY EARS] U+1F46F -> U+E429
  ["f09f91b0", :undef], # [BRIDE WITH VEIL] U+1F470 -> "[花嫁]"
  ["f09f91b1", "ee9495"], # [WESTERN PERSON] U+1F471 -> U+E515
  ["f09f91b2", "ee9496"], # [MAN WITH GUA PI MAO] U+1F472 -> U+E516
  ["f09f91b3", "ee9497"], # [MAN WITH TURBAN] U+1F473 -> U+E517
  ["f09f91b4", "ee9498"], # [OLDER MAN] U+1F474 -> U+E518
  ["f09f91b5", "ee9499"], # [OLDER WOMAN] U+1F475 -> U+E519
  ["f09f91b6", "ee949a"], # [BABY] U+1F476 -> U+E51A
  ["f09f91b7", "ee949b"], # [CONSTRUCTION WORKER] U+1F477 -> U+E51B
  ["f09f91b8", "ee949c"], # [PRINCESS] U+1F478 -> U+E51C
  ["f09f91b9", :undef], # [JAPANESE OGRE] U+1F479 -> "[なまはげ]"
  ["f09f91ba", :undef], # [JAPANESE GOBLIN] U+1F47A -> "[天狗]"
  ["f09f91bb", "ee849b"], # [GHOST] U+1F47B -> U+E11B
  ["f09f91bc", "ee818e"], # [BABY ANGEL] U+1F47C -> U+E04E
  ["f09f91bd", "ee848c"], # [EXTRATERRESTRIAL ALIEN] U+1F47D -> U+E10C
  ["f09f91be", "ee84ab"], # [ALIEN MONSTER] U+1F47E -> U+E12B
  ["f09f91bf", "ee849a"], # [IMP] U+1F47F -> U+E11A
  ["f09f9280", "ee849c"], # [SKULL] U+1F480 -> U+E11C
  ["f09f9281", "ee8993"], # [INFORMATION DESK PERSON] U+1F481 -> U+E253
  ["f09f9282", "ee949e"], # [GUARDSMAN] U+1F482 -> U+E51E
  ["f09f9283", "ee949f"], # [DANCER] U+1F483 -> U+E51F
  ["f09f908c", :undef], # [SNAIL] U+1F40C -> "[カタツムリ]"
  ["f09f908d", "ee94ad"], # [SNAKE] U+1F40D -> U+E52D
  ["f09f908e", "ee84b4"], # [HORSE] U+1F40E -> U+E134
  ["f09f9094", "ee94ae"], # [CHICKEN] U+1F414 -> U+E52E
  ["f09f9097", "ee94af"], # [BOAR] U+1F417 -> U+E52F
  ["f09f90ab", "ee94b0"], # [BACTRIAN CAMEL] U+1F42B -> U+E530
  ["f09f9098", "ee94a6"], # [ELEPHANT] U+1F418 -> U+E526
  ["f09f90a8", "ee94a7"], # [KOALA] U+1F428 -> U+E527
  ["f09f9092", "ee94a8"], # [MONKEY] U+1F412 -> U+E528
  ["f09f9091", "ee94a9"], # [SHEEP] U+1F411 -> U+E529
  ["f09f9099", "ee848a"], # [OCTOPUS] U+1F419 -> U+E10A
  ["f09f909a", "ee9181"], # [SPIRAL SHELL] U+1F41A -> U+E441
  ["f09f909b", "ee94a5"], # [BUG] U+1F41B -> U+E525
  ["f09f909c", :undef], # [ANT] U+1F41C -> "[アリ]"
  ["f09f909d", :undef], # [HONEYBEE] U+1F41D -> "[ミツバチ]"
  ["f09f909e", :undef], # [LADY BEETLE] U+1F41E -> "[てんとう虫]"
  ["f09f90a0", "ee94a2"], # [TROPICAL FISH] U+1F420 -> U+E522
  ["f09f90a1", "ee8099"], # [BLOWFISH] U+1F421 -> U+E019
  ["f09f90a2", :undef], # [TURTLE] U+1F422 -> "[カメ]"
  ["f09f90a4", "ee94a3"], # [BABY CHICK] U+1F424 -> U+E523
  ["f09f90a5", "ee94a3"], # [FRONT-FACING BABY CHICK] U+1F425 -> U+E523
  ["f09f90a6", "ee94a1"], # [BIRD] U+1F426 -> U+E521
  ["f09f90a3", "ee94a3"], # [HATCHING CHICK] U+1F423 -> U+E523
  ["f09f90a7", "ee8195"], # [PENGUIN] U+1F427 -> U+E055
  ["f09f90a9", "ee8192"], # [POODLE] U+1F429 -> U+E052
  ["f09f909f", "ee8099"], # [FISH] U+1F41F -> U+E019
  ["f09f90ac", "ee94a0"], # [DOLPHIN] U+1F42C -> U+E520
  ["f09f90ad", "ee8193"], # [MOUSE FACE] U+1F42D -> U+E053
  ["f09f90af", "ee8190"], # [TIGER FACE] U+1F42F -> U+E050
  ["f09f90b1", "ee818f"], # [CAT FACE] U+1F431 -> U+E04F
  ["f09f90b3", "ee8194"], # [SPOUTING WHALE] U+1F433 -> U+E054
  ["f09f90b4", "ee809a"], # [HORSE FACE] U+1F434 -> U+E01A
  ["f09f90b5", "ee8489"], # [MONKEY FACE] U+1F435 -> U+E109
  ["f09f90b6", "ee8192"], # [DOG FACE] U+1F436 -> U+E052
  ["f09f90b7", "ee848b"], # [PIG FACE] U+1F437 -> U+E10B
  ["f09f90bb", "ee8191"], # [BEAR FACE] U+1F43B -> U+E051
  ["f09f90b9", "ee94a4"], # [HAMSTER FACE] U+1F439 -> U+E524
  ["f09f90ba", "ee94aa"], # [WOLF FACE] U+1F43A -> U+E52A
  ["f09f90ae", "ee94ab"], # [COW FACE] U+1F42E -> U+E52B
  ["f09f90b0", "ee94ac"], # [RABBIT FACE] U+1F430 -> U+E52C
  ["f09f90b8", "ee94b1"], # [FROG FACE] U+1F438 -> U+E531
  ["f09f90be", "ee94b6"], # [PAW PRINTS] U+1F43E -> U+E536
  ["f09f90b2", :undef], # [DRAGON FACE] U+1F432 -> "[辰]"
  ["f09f90bc", :undef], # [PANDA FACE] U+1F43C -> "[パンダ]"
  ["f09f90bd", "ee848b"], # [PIG NOSE] U+1F43D -> U+E10B
  ["f09f9880", "ee8199"], # [ANGRY FACE] U+1F600 -> U+E059
  ["f09f9881", "ee9083"], # [ANGUISHED FACE] U+1F601 -> U+E403
  ["f09f9882", "ee9090"], # [ASTONISHED FACE] U+1F602 -> U+E410
  ["f09f9883", "ee8198"], # [DISAPPOINTED FACE] U+1F603 -> U+E058
  ["f09f9884", "ee9086"], # [DIZZY FACE] U+1F604 -> U+E406
  ["f09f9885", "ee908f"], # [EXASPERATED FACE] U+1F605 -> U+E40F
  ["f09f9886", "ee908e"], # [EXPRESSIONLESS FACE] U+1F606 -> U+E40E
  ["f09f9887", "ee8486"], # [FACE WITH HEART-SHAPED EYES] U+1F607 -> U+E106
  ["f09f9888", "ee9084"], # [FACE WITH LOOK OF TRIUMPH] U+1F608 -> U+E404
  ["f09f9889", "ee8485"], # [WINKING FACE WITH STUCK-OUT TONGUE] U+1F609 -> U+E105
  ["f09f988a", "ee9089"], # [FACE WITH STUCK-OUT TONGUE] U+1F60A -> U+E409
  ["f09f988b", "ee8196"], # [FACE SAVOURING DELICIOUS FOOD] U+1F60B -> U+E056
  ["f09f988c", "ee9098"], # [FACE THROWING A KISS] U+1F60C -> U+E418
  ["f09f988d", "ee9097"], # [FACE KISSING] U+1F60D -> U+E417
  ["f09f988e", "ee908c"], # [FACE WITH MASK] U+1F60E -> U+E40C
  ["f09f988f", "ee908d"], # [FLUSHED FACE] U+1F60F -> U+E40D
  ["f09f9890", "ee8197"], # [HAPPY FACE WITH OPEN MOUTH] U+1F610 -> U+E057
  ["f09f9891", "ee9095ee8cb1"], # [HAPPY FACE WITH OPEN MOUTH AND COLD SWEAT] U+1F611 -> U+E415 U+E331
  ["f09f9892", "ee908a"], # [HAPPY FACE WITH OPEN MOUTH AND CLOSED EYES] U+1F612 -> U+E40A
  ["f09f9893", "ee9084"], # [HAPPY FACE WITH GRIN] U+1F613 -> U+E404
  ["f09f9894", "ee9092"], # [HAPPY AND CRYING FACE] U+1F614 -> U+E412
  ["f09f9895", "ee8196"], # [HAPPY FACE WITH WIDE MOUTH AND RAISED EYEBROWS] U+1F615 -> U+E056
  ["e298ba", "ee9094"], # [WHITE SMILING FACE] U+263A -> U+E414
  ["f09f9896", "ee9095"], # [HAPPY FACE WITH OPEN MOUTH AND RAISED EYEBROWS] U+1F616 -> U+E415
  ["f09f9897", "ee9093"], # [CRYING FACE] U+1F617 -> U+E413
  ["f09f9898", "ee9091"], # [LOUDLY CRYING FACE] U+1F618 -> U+E411
  ["f09f9899", "ee908b"], # [FEARFUL FACE] U+1F619 -> U+E40B
  ["f09f989a", "ee9086"], # [PERSEVERING FACE] U+1F61A -> U+E406
  ["f09f989b", "ee9096"], # [POUTING FACE] U+1F61B -> U+E416
  ["f09f989c", "ee908a"], # [RELIEVED FACE] U+1F61C -> U+E40A
  ["f09f989d", "ee9087"], # [CONFOUNDED FACE] U+1F61D -> U+E407
  ["f09f989e", "ee9083"], # [PENSIVE FACE] U+1F61E -> U+E403
  ["f09f989f", "ee8487"], # [FACE SCREAMING IN FEAR] U+1F61F -> U+E107
  ["f09f98a0", "ee9088"], # [SLEEPY FACE] U+1F620 -> U+E408
  ["f09f98a1", "ee9082"], # [SMIRKING FACE] U+1F621 -> U+E402
  ["f09f98a2", "ee8488"], # [FACE WITH COLD SWEAT] U+1F622 -> U+E108
  ["f09f98a3", "ee9081"], # [DISAPPOINTED BUT RELIEVED FACE] U+1F623 -> U+E401
  ["f09f98a4", "ee9086"], # [TIRED FACE] U+1F624 -> U+E406
  ["f09f98a5", "ee9085"], # [WINKING FACE] U+1F625 -> U+E405
  ["f09f98ab", "ee8197"], # [CAT FACE WITH OPEN MOUTH] U+1F62B -> U+E057
  ["f09f98ac", "ee9084"], # [HAPPY CAT FACE WITH GRIN] U+1F62C -> U+E404
  ["f09f98ad", "ee9092"], # [HAPPY AND CRYING CAT FACE] U+1F62D -> U+E412
  ["f09f98ae", "ee9098"], # [CAT FACE KISSING] U+1F62E -> U+E418
  ["f09f98af", "ee8486"], # [CAT FACE WITH HEART-SHAPED EYES] U+1F62F -> U+E106
  ["f09f98b0", "ee9093"], # [CRYING CAT FACE] U+1F630 -> U+E413
  ["f09f98b1", "ee9096"], # [POUTING CAT FACE] U+1F631 -> U+E416
  ["f09f98b2", "ee9084"], # [CAT FACE WITH TIGHTLY-CLOSED LIPS] U+1F632 -> U+E404
  ["f09f98b3", "ee9083"], # [ANGUISHED CAT FACE] U+1F633 -> U+E403
  ["f09f98b4", "ee90a3"], # [FACE WITH NO GOOD GESTURE] U+1F634 -> U+E423
  ["f09f98b5", "ee90a4"], # [FACE WITH OK GESTURE] U+1F635 -> U+E424
  ["f09f98b6", "ee90a6"], # [PERSON BOWING DEEPLY] U+1F636 -> U+E426
  ["f09f98b7", :undef], # [SEE-NO-EVIL MONKEY] U+1F637 -> "(/_＼)"
  ["f09f98b9", :undef], # [SPEAK-NO-EVIL MONKEY] U+1F639 -> "(・×・)"
  ["f09f98b8", :undef], # [HEAR-NO-EVIL MONKEY] U+1F638 -> "|(・×・)|"
  ["f09f98ba", "ee8092"], # [PERSON RAISING ONE HAND] U+1F63A -> U+E012
  ["f09f98bb", "ee90a7"], # [PERSON RAISING BOTH HANDS IN CELEBRATION] U+1F63B -> U+E427
  ["f09f98bc", "ee9083"], # [PERSON FROWNING] U+1F63C -> U+E403
  ["f09f98bd", "ee9096"], # [PERSON WITH POUTING FACE] U+1F63D -> U+E416
  ["f09f98be", "ee909d"], # [PERSON WITH FOLDED HANDS] U+1F63E -> U+E41D
  ["f09f8fa0", "ee80b6"], # [HOUSE BUILDING] U+1F3E0 -> U+E036
  ["f09f8fa1", "ee80b6"], # [HOUSE WITH GARDEN] U+1F3E1 -> U+E036
  ["f09f8fa2", "ee80b8"], # [OFFICE BUILDING] U+1F3E2 -> U+E038
  ["f09f8fa3", "ee8593"], # [JAPANESE POST OFFICE] U+1F3E3 -> U+E153
  ["f09f8fa5", "ee8595"], # [HOSPITAL] U+1F3E5 -> U+E155
  ["f09f8fa6", "ee858d"], # [BANK] U+1F3E6 -> U+E14D
  ["f09f8fa7", "ee8594"], # [AUTOMATED TELLER MACHINE] U+1F3E7 -> U+E154
  ["f09f8fa8", "ee8598"], # [HOTEL] U+1F3E8 -> U+E158
  ["f09f8fa9", "ee9481"], # [LOVE HOTEL] U+1F3E9 -> U+E501
  ["f09f8faa", "ee8596"], # [CONVENIENCE STORE] U+1F3EA -> U+E156
  ["f09f8fab", "ee8597"], # [SCHOOL] U+1F3EB -> U+E157
  ["e29baa", "ee80b7"], # [CHURCH] U+26EA -> U+E037
  ["e29bb2", "ee84a1"], # [FOUNTAIN] U+26F2 -> U+E121
  ["f09f8fac", "ee9484"], # [DEPARTMENT STORE] U+1F3EC -> U+E504
  ["f09f8faf", "ee9485"], # [JAPANESE CASTLE] U+1F3EF -> U+E505
  ["f09f8fb0", "ee9486"], # [EUROPEAN CASTLE] U+1F3F0 -> U+E506
  ["f09f8fad", "ee9488"], # [FACTORY] U+1F3ED -> U+E508
  ["e29a93", "ee8882"], # [ANCHOR] U+2693 -> U+E202
  ["f09f8fae", "ee8c8b"], # [IZAKAYA LANTERN] U+1F3EE -> U+E30B
  ["f09f97bb", "ee80bb"], # [MOUNT FUJI] U+1F5FB -> U+E03B
  ["f09f97bc", "ee9489"], # [TOKYO TOWER] U+1F5FC -> U+E509
  ["f09f97bd", "ee949d"], # [STATUE OF LIBERTY] U+1F5FD -> U+E51D
  ["f09f97be", :undef], # [SILHOUETTE OF JAPAN] U+1F5FE -> "[日本地図]"
  ["f09f97bf", :undef], # [MOYAI] U+1F5FF -> "[モアイ]"
  ["f09f919e", "ee8087"], # [MANS SHOE] U+1F45E -> U+E007
  ["f09f919f", "ee8087"], # [ATHLETIC SHOE] U+1F45F -> U+E007
  ["f09f91a0", "ee84be"], # [HIGH-HEELED SHOE] U+1F460 -> U+E13E
  ["f09f91a1", "ee8c9a"], # [WOMANS SANDAL] U+1F461 -> U+E31A
  ["f09f91a2", "ee8c9b"], # [WOMANS BOOTS] U+1F462 -> U+E31B
  ["f09f91a3", "ee94b6"], # [FOOTPRINTS] U+1F463 -> U+E536
  ["f09f9193", :undef], # [EYEGLASSES] U+1F453 -> "[メガネ]"
  ["f09f9195", "ee8086"], # [T-SHIRT] U+1F455 -> U+E006
  ["f09f9196", :undef], # [JEANS] U+1F456 -> "[ジーンズ]"
  ["f09f9191", "ee848e"], # [CROWN] U+1F451 -> U+E10E
  ["f09f9194", "ee8c82"], # [NECKTIE] U+1F454 -> U+E302
  ["f09f9192", "ee8c98"], # [WOMANS HAT] U+1F452 -> U+E318
  ["f09f9197", "ee8c99"], # [DRESS] U+1F457 -> U+E319
  ["f09f9198", "ee8ca1"], # [KIMONO] U+1F458 -> U+E321
  ["f09f9199", "ee8ca2"], # [BIKINI] U+1F459 -> U+E322
  ["f09f919a", "ee8086"], # [WOMANS CLOTHES] U+1F45A -> U+E006
  ["f09f919b", :undef], # [PURSE] U+1F45B -> "[財布]"
  ["f09f919c", "ee8ca3"], # [HANDBAG] U+1F45C -> U+E323
  ["f09f919d", :undef], # [POUCH] U+1F45D -> "[ふくろ]"
  ["f09f92b0", "ee84af"], # [MONEY BAG] U+1F4B0 -> U+E12F
  ["f09f92b1", "ee8589"], # [CURRENCY EXCHANGE] U+1F4B1 -> U+E149
  ["f09f92b9", "ee858a"], # [CHART WITH UPWARDS TREND AND YEN SIGN] U+1F4B9 -> U+E14A
  ["f09f92b2", "ee84af"], # [HEAVY DOLLAR SIGN] U+1F4B2 -> U+E12F
  ["f09f92b3", :undef], # [CREDIT CARD] U+1F4B3 -> "[カード]"
  ["f09f92b4", :undef], # [BANKNOTE WITH YEN SIGN] U+1F4B4 -> "￥"
  ["f09f92b5", "ee84af"], # [BANKNOTE WITH DOLLAR SIGN] U+1F4B5 -> U+E12F
  ["f09f92b8", :undef], # [MONEY WITH WINGS] U+1F4B8 -> "[飛んでいくお金]"
  ["f09f87a6", :undef], # [REGIONAL INDICATOR SYMBOL LETTER A] U+1F1E6 -> "[A]"
  ["f09f87a7", :undef], # [REGIONAL INDICATOR SYMBOL LETTER B] U+1F1E7 -> "[B]"
  ["f09f87a8", :undef], # [REGIONAL INDICATOR SYMBOL LETTER C] U+1F1E8 -> "[C]"
  ["f09f87a9", :undef], # [REGIONAL INDICATOR SYMBOL LETTER D] U+1F1E9 -> "[D]"
  ["f09f87aa", :undef], # [REGIONAL INDICATOR SYMBOL LETTER E] U+1F1EA -> "[E]"
  ["f09f87ab", :undef], # [REGIONAL INDICATOR SYMBOL LETTER F] U+1F1EB -> "[F]"
  ["f09f87ac", :undef], # [REGIONAL INDICATOR SYMBOL LETTER G] U+1F1EC -> "[G]"
  ["f09f87ad", :undef], # [REGIONAL INDICATOR SYMBOL LETTER H] U+1F1ED -> "[H]"
  ["f09f87ae", :undef], # [REGIONAL INDICATOR SYMBOL LETTER I] U+1F1EE -> "[I]"
  ["f09f87af", :undef], # [REGIONAL INDICATOR SYMBOL LETTER J] U+1F1EF -> "[J]"
  ["f09f87b0", :undef], # [REGIONAL INDICATOR SYMBOL LETTER K] U+1F1F0 -> "[K]"
  ["f09f87b1", :undef], # [REGIONAL INDICATOR SYMBOL LETTER L] U+1F1F1 -> "[L]"
  ["f09f87b2", :undef], # [REGIONAL INDICATOR SYMBOL LETTER M] U+1F1F2 -> "[M]"
  ["f09f87b3", :undef], # [REGIONAL INDICATOR SYMBOL LETTER N] U+1F1F3 -> "[N]"
  ["f09f87b4", :undef], # [REGIONAL INDICATOR SYMBOL LETTER O] U+1F1F4 -> "[O]"
  ["f09f87b5", :undef], # [REGIONAL INDICATOR SYMBOL LETTER P] U+1F1F5 -> "[P]"
  ["f09f87b6", :undef], # [REGIONAL INDICATOR SYMBOL LETTER Q] U+1F1F6 -> "[Q]"
  ["f09f87b7", :undef], # [REGIONAL INDICATOR SYMBOL LETTER R] U+1F1F7 -> "[R]"
  ["f09f87b8", :undef], # [REGIONAL INDICATOR SYMBOL LETTER S] U+1F1F8 -> "[S]"
  ["f09f87b9", :undef], # [REGIONAL INDICATOR SYMBOL LETTER T] U+1F1F9 -> "[T]"
  ["f09f87ba", :undef], # [REGIONAL INDICATOR SYMBOL LETTER U] U+1F1FA -> "[U]"
  ["f09f87bb", :undef], # [REGIONAL INDICATOR SYMBOL LETTER V] U+1F1FB -> "[V]"
  ["f09f87bc", :undef], # [REGIONAL INDICATOR SYMBOL LETTER W] U+1F1FC -> "[W]"
  ["f09f87bd", :undef], # [REGIONAL INDICATOR SYMBOL LETTER X] U+1F1FD -> "[X]"
  ["f09f87be", :undef], # [REGIONAL INDICATOR SYMBOL LETTER Y] U+1F1FE -> "[Y]"
  ["f09f87bf", :undef], # [REGIONAL INDICATOR SYMBOL LETTER Z] U+1F1FF -> "[Z]"
  ["f09f94a5", "ee849d"], # [FIRE] U+1F525 -> U+E11D
  ["f09f94a6", :undef], # [ELECTRIC TORCH] U+1F526 -> "[懐中電灯]"
  ["f09f94a7", :undef], # [WRENCH] U+1F527 -> "[レンチ]"
  ["f09f94a8", "ee8496"], # [HAMMER] U+1F528 -> U+E116
  ["f09f94a9", :undef], # [NUT AND BOLT] U+1F529 -> "[ネジ]"
  ["f09f94aa", :undef], # [HOCHO] U+1F52A -> "[包丁]"
  ["f09f94ab", "ee8493"], # [PISTOL] U+1F52B -> U+E113
  ["f09f94ae", "ee88be"], # [CRYSTAL BALL] U+1F52E -> U+E23E
  ["f09f94af", "ee88be"], # [SIX POINTED STAR WITH MIDDLE DOT] U+1F52F -> U+E23E
  ["f09f94b0", "ee8889"], # [JAPANESE SYMBOL FOR BEGINNER] U+1F530 -> U+E209
  ["f09f94b1", "ee80b1"], # [TRIDENT EMBLEM] U+1F531 -> U+E031
  ["f09f9289", "ee84bb"], # [SYRINGE] U+1F489 -> U+E13B
  ["f09f928a", "ee8c8f"], # [PILL] U+1F48A -> U+E30F
  ["f09f85b0", "ee94b2"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER A] U+1F170 -> U+E532
  ["f09f85b1", "ee94b3"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER B] U+1F171 -> U+E533
  ["f09f868e", "ee94b4"], # [NEGATIVE SQUARED AB] U+1F18E -> U+E534
  ["f09f85be", "ee94b5"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER O] U+1F17E -> U+E535
  ["f09f8e80", "ee8c94"], # [RIBBON] U+1F380 -> U+E314
  ["f09f8e81", "ee8492"], # [WRAPPED PRESENT] U+1F381 -> U+E112
  ["f09f8e82", "ee8d8b"], # [BIRTHDAY CAKE] U+1F382 -> U+E34B
  ["f09f8e84", "ee80b3"], # [CHRISTMAS TREE] U+1F384 -> U+E033
  ["f09f8e85", "ee9188"], # [FATHER CHRISTMAS] U+1F385 -> U+E448
  ["f09f8e8c", "ee8583"], # [CROSSED FLAGS] U+1F38C -> U+E143
  ["f09f8e86", "ee8497"], # [FIREWORKS] U+1F386 -> U+E117
  ["f09f8e88", "ee8c90"], # [BALLOON] U+1F388 -> U+E310
  ["f09f8e89", "ee8c92"], # [PARTY POPPER] U+1F389 -> U+E312
  ["f09f8e8d", "ee90b6"], # [PINE DECORATION] U+1F38D -> U+E436
  ["f09f8e8e", "ee90b8"], # [JAPANESE DOLLS] U+1F38E -> U+E438
  ["f09f8e93", "ee90b9"], # [GRADUATION CAP] U+1F393 -> U+E439
  ["f09f8e92", "ee90ba"], # [SCHOOL SATCHEL] U+1F392 -> U+E43A
  ["f09f8e8f", "ee90bb"], # [CARP STREAMER] U+1F38F -> U+E43B
  ["f09f8e87", "ee9180"], # [FIREWORK SPARKLER] U+1F387 -> U+E440
  ["f09f8e90", "ee9182"], # [WIND CHIME] U+1F390 -> U+E442
  ["f09f8e83", "ee9185"], # [JACK-O-LANTERN] U+1F383 -> U+E445
  ["f09f8e8a", :undef], # [CONFETTI BALL] U+1F38A -> "[オメデトウ]"
  ["f09f8e8b", :undef], # [TANABATA TREE] U+1F38B -> "[七夕]"
  ["f09f8e91", "ee9186"], # [MOON VIEWING CEREMONY] U+1F391 -> U+E446
  ["f09f939f", :undef], # [PAGER] U+1F4DF -> "[ポケベル]"
  ["e2988e", "ee8089"], # [BLACK TELEPHONE] U+260E -> U+E009
  ["f09f939e", "ee8089"], # [TELEPHONE RECEIVER] U+1F4DE -> U+E009
  ["f09f93b1", "ee808a"], # [MOBILE PHONE] U+1F4F1 -> U+E00A
  ["f09f93b2", "ee8484"], # [MOBILE PHONE WITH RIGHTWARDS ARROW AT LEFT] U+1F4F2 -> U+E104
  ["f09f939d", "ee8c81"], # [MEMO] U+1F4DD -> U+E301
  ["f09f93a0", "ee808b"], # [FAX MACHINE] U+1F4E0 -> U+E00B
  ["e29c89", "ee8483"], # [ENVELOPE] U+2709 -> U+E103
  ["f09f93a8", "ee8483"], # [INCOMING ENVELOPE] U+1F4E8 -> U+E103
  ["f09f93a9", "ee8483"], # [ENVELOPE WITH DOWNWARDS ARROW ABOVE] U+1F4E9 -> U+E103
  ["f09f93aa", "ee8481"], # [CLOSED MAILBOX WITH LOWERED FLAG] U+1F4EA -> U+E101
  ["f09f93ab", "ee8481"], # [CLOSED MAILBOX WITH RAISED FLAG] U+1F4EB -> U+E101
  ["f09f93ae", "ee8482"], # [POSTBOX] U+1F4EE -> U+E102
  ["f09f93b0", :undef], # [NEWSPAPER] U+1F4F0 -> "[新聞]"
  ["f09f93a2", "ee8582"], # [PUBLIC ADDRESS LOUDSPEAKER] U+1F4E2 -> U+E142
  ["f09f93a3", "ee8c97"], # [CHEERING MEGAPHONE] U+1F4E3 -> U+E317
  ["f09f93a1", "ee858b"], # [SATELLITE ANTENNA] U+1F4E1 -> U+E14B
  ["f09f93a4", :undef], # [OUTBOX TRAY] U+1F4E4 -> "[送信BOX]"
  ["f09f93a5", :undef], # [INBOX TRAY] U+1F4E5 -> "[受信BOX]"
  ["f09f93a6", "ee8492"], # [PACKAGE] U+1F4E6 -> U+E112
  ["f09f93a7", "ee8483"], # [E-MAIL SYMBOL] U+1F4E7 -> U+E103
  ["f09f94a0", :undef], # [INPUT SYMBOL FOR LATIN CAPITAL LETTERS] U+1F520 -> "[ABCD]"
  ["f09f94a1", :undef], # [INPUT SYMBOL FOR LATIN SMALL LETTERS] U+1F521 -> "[abcd]"
  ["f09f94a2", :undef], # [INPUT SYMBOL FOR NUMBERS] U+1F522 -> "[1234]"
  ["f09f94a3", :undef], # [INPUT SYMBOL FOR SYMBOLS] U+1F523 -> "[記号]"
  ["f09f94a4", :undef], # [INPUT SYMBOL FOR LATIN LETTERS] U+1F524 -> "[ABC]"
  ["e29c92", :undef], # [BLACK NIB] U+2712 -> "[ペン]"
  ["f09f92ba", "ee849f"], # [SEAT] U+1F4BA -> U+E11F
  ["f09f92bb", "ee808c"], # [PERSONAL COMPUTER] U+1F4BB -> U+E00C
  ["e29c8f", "ee8c81"], # [PENCIL] U+270F -> U+E301
  ["f09f938e", :undef], # [PAPERCLIP] U+1F4CE -> "[クリップ]"
  ["f09f92bc", "ee849e"], # [BRIEFCASE] U+1F4BC -> U+E11E
  ["f09f92bd", "ee8c96"], # [MINIDISC] U+1F4BD -> U+E316
  ["f09f92be", "ee8c96"], # [FLOPPY DISK] U+1F4BE -> U+E316
  ["f09f92bf", "ee84a6"], # [OPTICAL DISC] U+1F4BF -> U+E126
  ["f09f9380", "ee84a7"], # [DVD] U+1F4C0 -> U+E127
  ["e29c82", "ee8c93"], # [BLACK SCISSORS] U+2702 -> U+E313
  ["f09f938d", :undef], # [ROUND PUSHPIN] U+1F4CD -> "[画びょう]"
  ["f09f9383", "ee8c81"], # [PAGE WITH CURL] U+1F4C3 -> U+E301
  ["f09f9384", "ee8c81"], # [PAGE FACING UP] U+1F4C4 -> U+E301
  ["f09f9385", :undef], # [CALENDAR] U+1F4C5 -> "[カレンダー]"
  ["f09f9381", :undef], # [FILE FOLDER] U+1F4C1 -> "[フォルダ]"
  ["f09f9382", :undef], # [OPEN FILE FOLDER] U+1F4C2 -> "[フォルダ]"
  ["f09f9393", "ee8588"], # [NOTEBOOK] U+1F4D3 -> U+E148
  ["f09f9396", "ee8588"], # [OPEN BOOK] U+1F4D6 -> U+E148
  ["f09f9394", "ee8588"], # [NOTEBOOK WITH DECORATIVE COVER] U+1F4D4 -> U+E148
  ["f09f9395", "ee8588"], # [CLOSED BOOK] U+1F4D5 -> U+E148
  ["f09f9397", "ee8588"], # [GREEN BOOK] U+1F4D7 -> U+E148
  ["f09f9398", "ee8588"], # [BLUE BOOK] U+1F4D8 -> U+E148
  ["f09f9399", "ee8588"], # [ORANGE BOOK] U+1F4D9 -> U+E148
  ["f09f939a", "ee8588"], # [BOOKS] U+1F4DA -> U+E148
  ["f09f939b", :undef], # [NAME BADGE] U+1F4DB -> "[名札]"
  ["f09f939c", :undef], # [SCROLL] U+1F4DC -> "[スクロール]"
  ["f09f938b", "ee8c81"], # [CLIPBOARD] U+1F4CB -> U+E301
  ["f09f9386", :undef], # [TEAR-OFF CALENDAR] U+1F4C6 -> "[カレンダー]"
  ["f09f938a", "ee858a"], # [BAR CHART] U+1F4CA -> U+E14A
  ["f09f9388", "ee858a"], # [CHART WITH UPWARDS TREND] U+1F4C8 -> U+E14A
  ["f09f9389", :undef], # [CHART WITH DOWNWARDS TREND] U+1F4C9 -> "[グラフ]"
  ["f09f9387", "ee8588"], # [CARD INDEX] U+1F4C7 -> U+E148
  ["f09f938c", :undef], # [PUSHPIN] U+1F4CC -> "[画びょう]"
  ["f09f9392", "ee8588"], # [LEDGER] U+1F4D2 -> U+E148
  ["f09f938f", :undef], # [STRAIGHT RULER] U+1F4CF -> "[定規]"
  ["f09f9390", :undef], # [TRIANGULAR RULER] U+1F4D0 -> "[三角定規]"
  ["f09f9391", "ee8c81"], # [BOOKMARK TABS] U+1F4D1 -> U+E301
  ["f09f8ebd", :undef], # [RUNNING SHIRT WITH SASH] U+1F3BD -> U+3013 (GETA)
  ["e29abe", "ee8096"], # [BASEBALL] U+26BE -> U+E016
  ["e29bb3", "ee8094"], # [FLAG IN HOLE] U+26F3 -> U+E014
  ["f09f8ebe", "ee8095"], # [TENNIS RACQUET AND BALL] U+1F3BE -> U+E015
  ["e29abd", "ee8098"], # [SOCCER BALL] U+26BD -> U+E018
  ["f09f8ebf", "ee8093"], # [SKI AND SKI BOOT] U+1F3BF -> U+E013
  ["f09f8f80", "ee90aa"], # [BASKETBALL AND HOOP] U+1F3C0 -> U+E42A
  ["f09f8f81", "ee84b2"], # [CHEQUERED FLAG] U+1F3C1 -> U+E132
  ["f09f8f82", :undef], # [SNOWBOARDER] U+1F3C2 -> "[スノボ]"
  ["f09f8f83", "ee8495"], # [RUNNER] U+1F3C3 -> U+E115
  ["f09f8f84", "ee8097"], # [SURFER] U+1F3C4 -> U+E017
  ["f09f8f86", "ee84b1"], # [TROPHY] U+1F3C6 -> U+E131
  ["f09f8f88", "ee90ab"], # [AMERICAN FOOTBALL] U+1F3C8 -> U+E42B
  ["f09f8f8a", "ee90ad"], # [SWIMMER] U+1F3CA -> U+E42D
  ["f09f9a86", "ee809e"], # [TRAIN] U+1F686 -> U+E01E
  ["f09f9a87", "ee90b4"], # [METRO] U+1F687 -> U+E434
  ["e29382", "ee90b4"], # [CIRCLED LATIN CAPITAL LETTER M] U+24C2 -> U+E434
  ["f09f9a84", "ee90b5"], # [HIGH-SPEED TRAIN] U+1F684 -> U+E435
  ["f09f9a85", "ee809f"], # [HIGH-SPEED TRAIN WITH BULLET NOSE] U+1F685 -> U+E01F
  ["f09f9a97", "ee809b"], # [AUTOMOBILE] U+1F697 -> U+E01B
  ["f09f9a99", "ee90ae"], # [RECREATIONAL VEHICLE] U+1F699 -> U+E42E
  ["f09f9a8d", "ee8599"], # [ONCOMING BUS] U+1F68D -> U+E159
  ["f09f9a8f", "ee8590"], # [BUS STOP] U+1F68F -> U+E150
  ["f09f9aa2", "ee8882"], # [SHIP] U+1F6A2 -> U+E202
  ["e29c88", "ee809d"], # [AIRPLANE] U+2708 -> U+E01D
  ["e29bb5", "ee809c"], # [SAILBOAT] U+26F5 -> U+E01C
  ["f09f9a89", "ee80b9"], # [STATION] U+1F689 -> U+E039
  ["f09f9a80", "ee848d"], # [ROCKET] U+1F680 -> U+E10D
  ["f09f9aa4", "ee84b5"], # [SPEEDBOAT] U+1F6A4 -> U+E135
  ["f09f9a95", "ee859a"], # [TAXI] U+1F695 -> U+E15A
  ["f09f9a9a", "ee90af"], # [DELIVERY TRUCK] U+1F69A -> U+E42F
  ["f09f9a92", "ee90b0"], # [FIRE ENGINE] U+1F692 -> U+E430
  ["f09f9a91", "ee90b1"], # [AMBULANCE] U+1F691 -> U+E431
  ["f09f9a93", "ee90b2"], # [POLICE CAR] U+1F693 -> U+E432
  ["e29bbd", "ee80ba"], # [FUEL PUMP] U+26FD -> U+E03A
  ["f09f85bf", "ee858f"], # [NEGATIVE SQUARED LATIN CAPITAL LETTER P] U+1F17F -> U+E14F
  ["f09f9aa5", "ee858e"], # [HORIZONTAL TRAFFIC LIGHT] U+1F6A5 -> U+E14E
  ["f09f9aa7", "ee84b7"], # [CONSTRUCTION SIGN] U+1F6A7 -> U+E137
  ["f09f9aa8", "ee90b2"], # [POLICE CARS REVOLVING LIGHT] U+1F6A8 -> U+E432
  ["e299a8", "ee84a3"], # [HOT SPRINGS] U+2668 -> U+E123
  ["e29bba", "ee84a2"], # [TENT] U+26FA -> U+E122
  ["f09f8ea0", :undef], # [CAROUSEL HORSE] U+1F3A0 -> U+3013 (GETA)
  ["f09f8ea1", "ee84a4"], # [FERRIS WHEEL] U+1F3A1 -> U+E124
  ["f09f8ea2", "ee90b3"], # [ROLLER COASTER] U+1F3A2 -> U+E433
  ["f09f8ea3", "ee8099"], # [FISHING POLE AND FISH] U+1F3A3 -> U+E019
  ["f09f8ea4", "ee80bc"], # [MICROPHONE] U+1F3A4 -> U+E03C
  ["f09f8ea5", "ee80bd"], # [MOVIE CAMERA] U+1F3A5 -> U+E03D
  ["f09f8ea6", "ee9487"], # [CINEMA] U+1F3A6 -> U+E507
  ["f09f8ea7", "ee8c8a"], # [HEADPHONE] U+1F3A7 -> U+E30A
  ["f09f8ea8", "ee9482"], # [ARTIST PALETTE] U+1F3A8 -> U+E502
  ["f09f8ea9", "ee9483"], # [TOP HAT] U+1F3A9 -> U+E503
  ["f09f8eaa", :undef], # [CIRCUS TENT] U+1F3AA -> "[イベント]"
  ["f09f8eab", "ee84a5"], # [TICKET] U+1F3AB -> U+E125
  ["f09f8eac", "ee8ca4"], # [CLAPPER BOARD] U+1F3AC -> U+E324
  ["f09f8ead", "ee9483"], # [PERFORMING ARTS] U+1F3AD -> U+E503
  ["f09f8eae", :undef], # [VIDEO GAME] U+1F3AE -> "[ゲーム]"
  ["f09f8084", "ee84ad"], # [MAHJONG TILE RED DRAGON] U+1F004 -> U+E12D
  ["f09f8eaf", "ee84b0"], # [DIRECT HIT] U+1F3AF -> U+E130
  ["f09f8eb0", "ee84b3"], # [SLOT MACHINE] U+1F3B0 -> U+E133
  ["f09f8eb1", "ee90ac"], # [BILLIARDS] U+1F3B1 -> U+E42C
  ["f09f8eb2", :undef], # [GAME DIE] U+1F3B2 -> "[サイコロ]"
  ["f09f8eb3", :undef], # [BOWLING] U+1F3B3 -> "[ボーリング]"
  ["f09f8eb4", :undef], # [FLOWER PLAYING CARDS] U+1F3B4 -> "[花札]"
  ["f09f838f", :undef], # [PLAYING CARD BLACK JOKER] U+1F0CF -> "[ジョーカー]"
  ["f09f8eb5", "ee80be"], # [MUSICAL NOTE] U+1F3B5 -> U+E03E
  ["f09f8eb6", "ee8ca6"], # [MULTIPLE MUSICAL NOTES] U+1F3B6 -> U+E326
  ["f09f8eb7", "ee8180"], # [SAXOPHONE] U+1F3B7 -> U+E040
  ["f09f8eb8", "ee8181"], # [GUITAR] U+1F3B8 -> U+E041
  ["f09f8eb9", :undef], # [MUSICAL KEYBOARD] U+1F3B9 -> "[ピアノ]"
  ["f09f8eba", "ee8182"], # [TRUMPET] U+1F3BA -> U+E042
  ["f09f8ebb", :undef], # [VIOLIN] U+1F3BB -> "[バイオリン]"
  ["f09f8ebc", "ee8ca6"], # [MUSICAL SCORE] U+1F3BC -> U+E326
  ["e380bd", "ee84ac"], # [PART ALTERNATION MARK] U+303D -> U+E12C
  ["f09f93b7", "ee8088"], # [CAMERA] U+1F4F7 -> U+E008
  ["f09f93b9", "ee80bd"], # [VIDEO CAMERA] U+1F4F9 -> U+E03D
  ["f09f93ba", "ee84aa"], # [TELEVISION] U+1F4FA -> U+E12A
  ["f09f93bb", "ee84a8"], # [RADIO] U+1F4FB -> U+E128
  ["f09f93bc", "ee84a9"], # [VIDEOCASSETTE] U+1F4FC -> U+E129
  ["f09f928b", "ee8083"], # [KISS MARK] U+1F48B -> U+E003
  ["f09f928c", "ee8483ee8ca8"], # [LOVE LETTER] U+1F48C -> U+E103 U+E328
  ["f09f928d", "ee80b4"], # [RING] U+1F48D -> U+E034
  ["f09f928e", "ee80b5"], # [GEM STONE] U+1F48E -> U+E035
  ["f09f928f", "ee8491"], # [KISS] U+1F48F -> U+E111
  ["f09f9290", "ee8c86"], # [BOUQUET] U+1F490 -> U+E306
  ["f09f9291", "ee90a5"], # [COUPLE WITH HEART] U+1F491 -> U+E425
  ["f09f9292", "ee90bd"], # [WEDDING] U+1F492 -> U+E43D
  ["f09f949e", "ee8887"], # [NO ONE UNDER EIGHTEEN SYMBOL] U+1F51E -> U+E207
  ["c2a9", "ee898e"], # [COPYRIGHT SIGN] U+A9 -> U+E24E
  ["c2ae", "ee898f"], # [REGISTERED SIGN] U+AE -> U+E24F
  ["e284a2", "ee94b7"], # [TRADE MARK SIGN] U+2122 -> U+E537
  ["e284b9", :undef], # [INFORMATION SOURCE] U+2139 -> "[ｉ]"
  ["f09f949f", :undef], # [KEYCAP TEN] U+1F51F -> "[10]"
  ["f09f93b6", "ee888b"], # [ANTENNA WITH BARS] U+1F4F6 -> U+E20B
  ["f09f93b3", "ee8990"], # [VIBRATION MODE] U+1F4F3 -> U+E250
  ["f09f93b4", "ee8991"], # [MOBILE PHONE OFF] U+1F4F4 -> U+E251
  ["f09f8d94", "ee84a0"], # [HAMBURGER] U+1F354 -> U+E120
  ["f09f8d99", "ee8d82"], # [RICE BALL] U+1F359 -> U+E342
  ["f09f8db0", "ee8186"], # [SHORTCAKE] U+1F370 -> U+E046
  ["f09f8d9c", "ee8d80"], # [STEAMING BOWL] U+1F35C -> U+E340
  ["f09f8d9e", "ee8cb9"], # [BREAD] U+1F35E -> U+E339
  ["f09f8db3", "ee8587"], # [COOKING] U+1F373 -> U+E147
  ["f09f8da6", "ee8cba"], # [SOFT ICE CREAM] U+1F366 -> U+E33A
  ["f09f8d9f", "ee8cbb"], # [FRENCH FRIES] U+1F35F -> U+E33B
  ["f09f8da1", "ee8cbc"], # [DANGO] U+1F361 -> U+E33C
  ["f09f8d98", "ee8cbd"], # [RICE CRACKER] U+1F358 -> U+E33D
  ["f09f8d9a", "ee8cbe"], # [COOKED RICE] U+1F35A -> U+E33E
  ["f09f8d9d", "ee8cbf"], # [SPAGHETTI] U+1F35D -> U+E33F
  ["f09f8d9b", "ee8d81"], # [CURRY AND RICE] U+1F35B -> U+E341
  ["f09f8da2", "ee8d83"], # [ODEN] U+1F362 -> U+E343
  ["f09f8da3", "ee8d84"], # [SUSHI] U+1F363 -> U+E344
  ["f09f8db1", "ee8d8c"], # [BENTO BOX] U+1F371 -> U+E34C
  ["f09f8db2", "ee8d8d"], # [POT OF FOOD] U+1F372 -> U+E34D
  ["f09f8da7", "ee90bf"], # [SHAVED ICE] U+1F367 -> U+E43F
  ["f09f8d96", :undef], # [MEAT ON BONE] U+1F356 -> "[肉]"
  ["f09f8da5", :undef], # [FISH CAKE WITH SWIRL DESIGN] U+1F365 -> "[なると]"
  ["f09f8da0", :undef], # [ROASTED SWEET POTATO] U+1F360 -> "[やきいも]"
  ["f09f8d95", :undef], # [SLICE OF PIZZA] U+1F355 -> "[ピザ]"
  ["f09f8d97", :undef], # [POULTRY LEG] U+1F357 -> "[チキン]"
  ["f09f8da8", :undef], # [ICE CREAM] U+1F368 -> "[アイスクリーム]"
  ["f09f8da9", :undef], # [DOUGHNUT] U+1F369 -> "[ドーナツ]"
  ["f09f8daa", :undef], # [COOKIE] U+1F36A -> "[クッキー]"
  ["f09f8dab", :undef], # [CHOCOLATE BAR] U+1F36B -> "[チョコ]"
  ["f09f8dac", :undef], # [CANDY] U+1F36C -> "[キャンディ]"
  ["f09f8dad", :undef], # [LOLLIPOP] U+1F36D -> "[キャンディ]"
  ["f09f8dae", :undef], # [CUSTARD] U+1F36E -> "[プリン]"
  ["f09f8daf", :undef], # [HONEY POT] U+1F36F -> "[ハチミツ]"
  ["f09f8da4", :undef], # [FRIED SHRIMP] U+1F364 -> "[エビフライ]"
  ["f09f8db4", "ee8183"], # [FORK AND KNIFE] U+1F374 -> U+E043
  ["e29895", "ee8185"], # [HOT BEVERAGE] U+2615 -> U+E045
  ["f09f8db8", "ee8184"], # [COCKTAIL GLASS] U+1F378 -> U+E044
  ["f09f8dba", "ee8187"], # [BEER MUG] U+1F37A -> U+E047
  ["f09f8db5", "ee8cb8"], # [TEACUP WITHOUT HANDLE] U+1F375 -> U+E338
  ["f09f8db6", "ee8c8b"], # [SAKE BOTTLE AND CUP] U+1F376 -> U+E30B
  ["f09f8db7", "ee8184"], # [WINE GLASS] U+1F377 -> U+E044
  ["f09f8dbb", "ee8c8c"], # [CLINKING BEER MUGS] U+1F37B -> U+E30C
  ["f09f8db9", "ee8184"], # [TROPICAL DRINK] U+1F379 -> U+E044
  ["e28697", "ee88b6"], # [NORTH EAST ARROW] U+2197 -> U+E236
  ["e28698", "ee88b8"], # [SOUTH EAST ARROW] U+2198 -> U+E238
  ["e28696", "ee88b7"], # [NORTH WEST ARROW] U+2196 -> U+E237
  ["e28699", "ee88b9"], # [SOUTH WEST ARROW] U+2199 -> U+E239
  ["e2a4b4", "ee88b6"], # [ARROW POINTING RIGHTWARDS THEN CURVING UPWARDS] U+2934 -> U+E236
  ["e2a4b5", "ee88b8"], # [ARROW POINTING RIGHTWARDS THEN CURVING DOWNWARDS] U+2935 -> U+E238
  ["e28694", :undef], # [LEFT RIGHT ARROW] U+2194 -> "⇔"
  ["e28695", :undef], # [UP DOWN ARROW] U+2195 -> "↑↓"
  ["e2ac86", "ee88b2"], # [UPWARDS BLACK ARROW] U+2B06 -> U+E232
  ["e2ac87", "ee88b3"], # [DOWNWARDS BLACK ARROW] U+2B07 -> U+E233
  ["e29ea1", "ee88b4"], # [BLACK RIGHTWARDS ARROW] U+27A1 -> U+E234
  ["e2ac85", "ee88b5"], # [LEFTWARDS BLACK ARROW] U+2B05 -> U+E235
  ["e296b6", "ee88ba"], # [BLACK RIGHT-POINTING TRIANGLE] U+25B6 -> U+E23A
  ["e29780", "ee88bb"], # [BLACK LEFT-POINTING TRIANGLE] U+25C0 -> U+E23B
  ["e28fa9", "ee88bc"], # [BLACK RIGHT-POINTING DOUBLE TRIANGLE] U+23E9 -> U+E23C
  ["e28faa", "ee88bd"], # [BLACK LEFT-POINTING DOUBLE TRIANGLE] U+23EA -> U+E23D
  ["e28fab", :undef], # [BLACK UP-POINTING DOUBLE TRIANGLE] U+23EB -> "▲"
  ["e28fac", :undef], # [BLACK DOWN-POINTING DOUBLE TRIANGLE] U+23EC -> "▼"
  ["f09f94ba", :undef], # [UP-POINTING RED TRIANGLE] U+1F53A -> "▲"
  ["f09f94bb", :undef], # [DOWN-POINTING RED TRIANGLE] U+1F53B -> "▼"
  ["f09f94bc", :undef], # [UP-POINTING SMALL RED TRIANGLE] U+1F53C -> "▲"
  ["f09f94bd", :undef], # [DOWN-POINTING SMALL RED TRIANGLE] U+1F53D -> "▼"
  ["e2ad95", "ee8cb2"], # [HEAVY LARGE CIRCLE] U+2B55 -> U+E332
  ["e29d8c", "ee8cb3"], # [CROSS MARK] U+274C -> U+E333
  ["e29d8e", "ee8cb3"], # [NEGATIVE SQUARED CROSS MARK] U+274E -> U+E333
  ["e29da2", "ee80a1"], # [HEAVY EXCLAMATION MARK ORNAMENT] U+2762 -> U+E021
  ["e28189", :undef], # [EXCLAMATION QUESTION MARK] U+2049 -> "！？"
  ["e280bc", :undef], # [DOUBLE EXCLAMATION MARK] U+203C -> "！！"
  ["e29d93", "ee80a0"], # [BLACK QUESTION MARK ORNAMENT] U+2753 -> U+E020
  ["e29d94", "ee8cb6"], # [WHITE QUESTION MARK ORNAMENT] U+2754 -> U+E336
  ["e29d95", "ee8cb7"], # [WHITE EXCLAMATION MARK ORNAMENT] U+2755 -> U+E337
  ["e380b0", :undef], # [WAVY DASH] U+3030 -> U+3013 (GETA)
  ["e29eb0", :undef], # [CURLY LOOP] U+27B0 -> "～"
  ["e29ebf", "ee8891"], # [DOUBLE CURLY LOOP] U+27BF -> U+E211
  ["e29da4", "ee80a2"], # [HEAVY BLACK HEART] U+2764 -> U+E022
  ["f09f9293", "ee8ca7"], # [BEATING HEART] U+1F493 -> U+E327
  ["f09f9294", "ee80a3"], # [BROKEN HEART] U+1F494 -> U+E023
  ["f09f9295", "ee8ca7"], # [TWO HEARTS] U+1F495 -> U+E327
  ["f09f9296", "ee8ca7"], # [SPARKLING HEART] U+1F496 -> U+E327
  ["f09f9297", "ee8ca8"], # [GROWING HEART] U+1F497 -> U+E328
  ["f09f9298", "ee8ca9"], # [HEART WITH ARROW] U+1F498 -> U+E329
  ["f09f9299", "ee8caa"], # [BLUE HEART] U+1F499 -> U+E32A
  ["f09f929a", "ee8cab"], # [GREEN HEART] U+1F49A -> U+E32B
  ["f09f929b", "ee8cac"], # [YELLOW HEART] U+1F49B -> U+E32C
  ["f09f929c", "ee8cad"], # [PURPLE HEART] U+1F49C -> U+E32D
  ["f09f929d", "ee90b7"], # [HEART WITH RIBBON] U+1F49D -> U+E437
  ["f09f929e", "ee8ca7"], # [REVOLVING HEARTS] U+1F49E -> U+E327
  ["f09f929f", "ee8884"], # [HEART DECORATION] U+1F49F -> U+E204
  ["e299a5", "ee888c"], # [BLACK HEART SUIT] U+2665 -> U+E20C
  ["e299a0", "ee888e"], # [BLACK SPADE SUIT] U+2660 -> U+E20E
  ["e299a6", "ee888d"], # [BLACK DIAMOND SUIT] U+2666 -> U+E20D
  ["e299a3", "ee888f"], # [BLACK CLUB SUIT] U+2663 -> U+E20F
  ["f09f9aac", "ee8c8e"], # [SMOKING SYMBOL] U+1F6AC -> U+E30E
  ["f09f9aad", "ee8888"], # [NO SMOKING SYMBOL] U+1F6AD -> U+E208
  ["e299bf", "ee888a"], # [WHEELCHAIR SYMBOL] U+267F -> U+E20A
  ["f09f9aa9", :undef], # [TRIANGULAR FLAG ON POST] U+1F6A9 -> "[旗]"
  ["e29aa0", "ee8992"], # [WARNING SIGN] U+26A0 -> U+E252
  ["e29b94", "ee84b7"], # [NO ENTRY] U+26D4 -> U+E137
  ["e299bb", :undef], # [BLACK UNIVERSAL RECYCLING SYMBOL] U+267B -> "↑↓"
  ["f09f9ab2", "ee84b6"], # [BICYCLE] U+1F6B2 -> U+E136
  ["f09f9ab6", "ee8881"], # [PEDESTRIAN] U+1F6B6 -> U+E201
  ["f09f9ab9", "ee84b8"], # [MENS SYMBOL] U+1F6B9 -> U+E138
  ["f09f9aba", "ee84b9"], # [WOMENS SYMBOL] U+1F6BA -> U+E139
  ["f09f9b80", "ee84bf"], # [BATH] U+1F6C0 -> U+E13F
  ["f09f9abb", "ee8591"], # [RESTROOM] U+1F6BB -> U+E151
  ["f09f9abd", "ee8580"], # [TOILET] U+1F6BD -> U+E140
  ["f09f9abe", "ee8c89"], # [WATER CLOSET] U+1F6BE -> U+E309
  ["f09f9abc", "ee84ba"], # [BABY SYMBOL] U+1F6BC -> U+E13A
  ["f09f9aaa", :undef], # [DOOR] U+1F6AA -> "[ドア]"
  ["f09f9aab", :undef], # [NO ENTRY SIGN] U+1F6AB -> "[禁止]"
  ["e29c94", :undef], # [HEAVY CHECK MARK] U+2714 -> "[チェックマーク]"
  ["f09f8691", :undef], # [SQUARED CL] U+1F191 -> "[CL]"
  ["f09f8692", "ee8894"], # [SQUARED COOL] U+1F192 -> U+E214
  ["f09f8693", :undef], # [SQUARED FREE] U+1F193 -> "[FREE]"
  ["f09f8694", "ee88a9"], # [SQUARED ID] U+1F194 -> U+E229
  ["f09f8695", "ee8892"], # [SQUARED NEW] U+1F195 -> U+E212
  ["f09f8696", :undef], # [SQUARED NG] U+1F196 -> "[NG]"
  ["f09f8697", "ee898d"], # [SQUARED OK] U+1F197 -> U+E24D
  ["f09f8698", :undef], # [SQUARED SOS] U+1F198 -> "[SOS]"
  ["f09f8699", "ee8893"], # [SQUARED UP WITH EXCLAMATION MARK] U+1F199 -> U+E213
  ["f09f869a", "ee84ae"], # [SQUARED VS] U+1F19A -> U+E12E
  ["f09f8881", "ee8883"], # [SQUARED KATAKANA KOKO] U+1F201 -> U+E203
  ["f09f8882", "ee88a8"], # [SQUARED KATAKANA SA] U+1F202 -> U+E228
  ["f09f88b2", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-7981] U+1F232 -> "[禁]"
  ["f09f88b3", "ee88ab"], # [SQUARED CJK UNIFIED IDEOGRAPH-7A7A] U+1F233 -> U+E22B
  ["f09f88b4", :undef], # [SQUARED CJK UNIFIED IDEOGRAPH-5408] U+1F234 -> "[合]"
  ["f09f88b5", "ee88aa"], # [SQUARED CJK UNIFIED IDEOGRAPH-6E80] U+1F235 -> U+E22A
  ["f09f88b6", "ee8895"], # [SQUARED CJK UNIFIED IDEOGRAPH-6709] U+1F236 -> U+E215
  ["f09f889a", "ee8896"], # [SQUARED CJK UNIFIED IDEOGRAPH-7121] U+1F21A -> U+E216
  ["f09f88b7", "ee8897"], # [SQUARED CJK UNIFIED IDEOGRAPH-6708] U+1F237 -> U+E217
  ["f09f88b8", "ee8898"], # [SQUARED CJK UNIFIED IDEOGRAPH-7533] U+1F238 -> U+E218
  ["f09f88b9", "ee88a7"], # [SQUARED CJK UNIFIED IDEOGRAPH-5272] U+1F239 -> U+E227
  ["f09f88af", "ee88ac"], # [SQUARED CJK UNIFIED IDEOGRAPH-6307] U+1F22F -> U+E22C
  ["f09f88ba", "ee88ad"], # [SQUARED CJK UNIFIED IDEOGRAPH-55B6] U+1F23A -> U+E22D
  ["e38a99", "ee8c95"], # [CIRCLED IDEOGRAPH SECRET] U+3299 -> U+E315
  ["e38a97", "ee8c8d"], # [CIRCLED IDEOGRAPH CONGRATULATION] U+3297 -> U+E30D
  ["f09f8990", "ee88a6"], # [CIRCLED IDEOGRAPH ADVANTAGE] U+1F250 -> U+E226
  ["f09f8991", :undef], # [CIRCLED IDEOGRAPH ACCEPT] U+1F251 -> "[可]"
  ["e29e95", :undef], # [HEAVY PLUS SIGN] U+2795 -> "[＋]"
  ["e29e96", :undef], # [HEAVY MINUS SIGN] U+2796 -> "[－]"
  ["e29c96", "ee8cb3"], # [HEAVY MULTIPLICATION X] U+2716 -> U+E333
  ["e29e97", :undef], # [HEAVY DIVISION SIGN] U+2797 -> "[÷]"
  ["f09f92a0", :undef], # [DIAMOND SHAPE WITH A DOT INSIDE] U+1F4A0 -> U+3013 (GETA)
  ["f09f92a1", "ee848f"], # [ELECTRIC LIGHT BULB] U+1F4A1 -> U+E10F
  ["f09f92a2", "ee8cb4"], # [ANGER SYMBOL] U+1F4A2 -> U+E334
  ["f09f92a3", "ee8c91"], # [BOMB] U+1F4A3 -> U+E311
  ["f09f92a4", "ee84bc"], # [SLEEPING SYMBOL] U+1F4A4 -> U+E13C
  ["f09f92a5", :undef], # [COLLISION SYMBOL] U+1F4A5 -> "[ドンッ]"
  ["f09f92a6", "ee8cb1"], # [SPLASHING SWEAT SYMBOL] U+1F4A6 -> U+E331
  ["f09f92a7", "ee8cb1"], # [DROP OF WATER] U+1F4A7 -> U+E331
  ["f09f92a8", "ee8cb0"], # [DASH SYMBOL] U+1F4A8 -> U+E330
  ["f09f92a9", "ee819a"], # [PILE OF POO] U+1F4A9 -> U+E05A
  ["f09f92aa", "ee858c"], # [FLEXED BICEPS] U+1F4AA -> U+E14C
  ["f09f92ab", "ee9087"], # [DIZZY SYMBOL] U+1F4AB -> U+E407
  ["f09f92ac", :undef], # [SPEECH BALLOON] U+1F4AC -> "[フキダシ]"
  ["e29ca8", "ee8cae"], # [SPARKLES] U+2728 -> U+E32E
  ["e29cb4", "ee8885"], # [EIGHT POINTED BLACK STAR] U+2734 -> U+E205
  ["e29cb3", "ee8886"], # [EIGHT SPOKED ASTERISK] U+2733 -> U+E206
  ["e29aaa", "ee8899"], # [MEDIUM WHITE CIRCLE] U+26AA -> U+E219
  ["e29aab", "ee8899"], # [MEDIUM BLACK CIRCLE] U+26AB -> U+E219
  ["f09f94b4", "ee8899"], # [LARGE RED CIRCLE] U+1F534 -> U+E219
  ["f09f94b5", "ee889a"], # [LARGE BLUE CIRCLE] U+1F535 -> U+E21A
  ["f09f94b2", "ee889a"], # [BLACK SQUARE BUTTON] U+1F532 -> U+E21A
  ["f09f94b3", "ee889b"], # [WHITE SQUARE BUTTON] U+1F533 -> U+E21B
  ["e2ad90", "ee8caf"], # [WHITE MEDIUM STAR] U+2B50 -> U+E32F
  ["e2ac9c", "ee889b"], # [WHITE LARGE SQUARE] U+2B1C -> U+E21B
  ["e2ac9b", "ee889a"], # [BLACK LARGE SQUARE] U+2B1B -> U+E21A
  ["e296ab", "ee889b"], # [WHITE SMALL SQUARE] U+25AB -> U+E21B
  ["e296aa", "ee889a"], # [BLACK SMALL SQUARE] U+25AA -> U+E21A
  ["e297bd", "ee889b"], # [WHITE MEDIUM SMALL SQUARE] U+25FD -> U+E21B
  ["e297be", "ee889a"], # [BLACK MEDIUM SMALL SQUARE] U+25FE -> U+E21A
  ["e297bb", "ee889b"], # [WHITE MEDIUM SQUARE] U+25FB -> U+E21B
  ["e297bc", "ee889a"], # [BLACK MEDIUM SQUARE] U+25FC -> U+E21A
  ["f09f94b6", "ee889b"], # [LARGE ORANGE DIAMOND] U+1F536 -> U+E21B
  ["f09f94b7", "ee889b"], # [LARGE BLUE DIAMOND] U+1F537 -> U+E21B
  ["f09f94b8", "ee889b"], # [SMALL ORANGE DIAMOND] U+1F538 -> U+E21B
  ["f09f94b9", "ee889b"], # [SMALL BLUE DIAMOND] U+1F539 -> U+E21B
  ["e29d87", "ee8cae"], # [SPARKLE] U+2747 -> U+E32E
  ["f09f92ae", :undef], # [WHITE FLOWER] U+1F4AE -> "[花丸]"
  ["f09f92af", :undef], # [HUNDRED POINTS SYMBOL] U+1F4AF -> "[100点]"
  ["e286a9", :undef], # [LEFTWARDS ARROW WITH HOOK] U+21A9 -> "←┘"
  ["e286aa", :undef], # [RIGHTWARDS ARROW WITH HOOK] U+21AA -> "└→"
  ["f09f9483", :undef], # [CLOCKWISE DOWNWARDS AND UPWARDS OPEN CIRCLE ARROWS] U+1F503 -> "↑↓"
  ["f09f948a", "ee8581"], # [SPEAKER WITH THREE SOUND WAVES] U+1F50A -> U+E141
  ["f09f948b", :undef], # [BATTERY] U+1F50B -> "[電池]"
  ["f09f948c", :undef], # [ELECTRIC PLUG] U+1F50C -> "[コンセント]"
  ["f09f948d", "ee8494"], # [LEFT-POINTING MAGNIFYING GLASS] U+1F50D -> U+E114
  ["f09f948e", "ee8494"], # [RIGHT-POINTING MAGNIFYING GLASS] U+1F50E -> U+E114
  ["f09f9492", "ee8584"], # [LOCK] U+1F512 -> U+E144
  ["f09f9493", "ee8585"], # [OPEN LOCK] U+1F513 -> U+E145
  ["f09f948f", "ee8584"], # [LOCK WITH INK PEN] U+1F50F -> U+E144
  ["f09f9490", "ee8584"], # [CLOSED LOCK WITH KEY] U+1F510 -> U+E144
  ["f09f9491", "ee80bf"], # [KEY] U+1F511 -> U+E03F
  ["f09f9494", "ee8ca5"], # [BELL] U+1F514 -> U+E325
  ["e29891", :undef], # [BALLOT BOX WITH CHECK] U+2611 -> "[チェックマーク]"
  ["f09f9498", :undef], # [RADIO BUTTON] U+1F518 -> "[ラジオボタン]"
  ["f09f9496", :undef], # [BOOKMARK] U+1F516 -> "[ブックマーク]"
  ["f09f9497", :undef], # [LINK SYMBOL] U+1F517 -> "[リンク]"
  ["f09f9499", "ee88b5"], # [BACK WITH LEFTWARDS ARROW ABOVE] U+1F519 -> U+E235
  ["f09f949a", :undef], # [END WITH LEFTWARDS ARROW ABOVE] U+1F51A -> "[end]"
  ["f09f949b", :undef], # [ON WITH EXCLAMATION MARK WITH LEFT RIGHT ARROW ABOVE] U+1F51B -> "[ON]"
  ["f09f949c", :undef], # [SOON WITH RIGHTWARDS ARROW ABOVE] U+1F51C -> "[SOON]"
  ["f09f949d", "ee898c"], # [TOP WITH UPWARDS ARROW ABOVE] U+1F51D -> U+E24C
  ["e28083", :undef], # [EM SPACE] U+2003 -> U+3013 (GETA)
  ["e28082", :undef], # [EN SPACE] U+2002 -> U+3013 (GETA)
  ["e28085", :undef], # [FOUR-PER-EM SPACE] U+2005 -> U+3013 (GETA)
  ["e29c85", :undef], # [WHITE HEAVY CHECK MARK] U+2705 -> "[チェックマーク]"
  ["e29c8a", "ee8090"], # [RAISED FIST] U+270A -> U+E010
  ["e29c8b", "ee8092"], # [RAISED HAND] U+270B -> U+E012
  ["e29c8c", "ee8091"], # [VICTORY HAND] U+270C -> U+E011
  ["f09f918a", "ee808d"], # [FISTED HAND SIGN] U+1F44A -> U+E00D
  ["f09f918d", "ee808e"], # [THUMBS UP SIGN] U+1F44D -> U+E00E
  ["e2989d", "ee808f"], # [WHITE UP POINTING INDEX] U+261D -> U+E00F
  ["f09f9186", "ee88ae"], # [WHITE UP POINTING BACKHAND INDEX] U+1F446 -> U+E22E
  ["f09f9187", "ee88af"], # [WHITE DOWN POINTING BACKHAND INDEX] U+1F447 -> U+E22F
  ["f09f9188", "ee88b0"], # [WHITE LEFT POINTING BACKHAND INDEX] U+1F448 -> U+E230
  ["f09f9189", "ee88b1"], # [WHITE RIGHT POINTING BACKHAND INDEX] U+1F449 -> U+E231
  ["f09f918b", "ee909e"], # [WAVING HAND SIGN] U+1F44B -> U+E41E
  ["f09f918f", "ee909f"], # [CLAPPING HANDS SIGN] U+1F44F -> U+E41F
  ["f09f918c", "ee90a0"], # [OK HAND SIGN] U+1F44C -> U+E420
  ["f09f918e", "ee90a1"], # [THUMBS DOWN SIGN] U+1F44E -> U+E421
  ["f09f9190", "ee90a2"], # [OPEN HANDS SIGN] U+1F450 -> U+E422
]

