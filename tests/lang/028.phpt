--TEST--
Testing calling user-level functions from C
--FILE--
<?php

error_reporting(1023);

function print_stuff($stuff)
{
    print $stuff;
}


function still_working()
{
    return "I'm still alive";
}

function dafna()
{
    static $foo = 0;

    print "Dafna!\n";
    print call_user_func("still_working")."\n";
    $foo++;
    return (string) $foo;
}


class dafna_class {
    function __construct() {
        $this->myname = "Dafna";
    }
    function GetMyName() {
        return $this->myname;
    }
    function SetMyName($name) {
        $this->myname = $name;
    }
};

for ($i=0; $i<200; $i++):
    print "$i\n";
    call_user_func("dafna");
    call_user_func("print_stuff","Hey there!!\n");
    print "$i\n";
endfor;


$dafna = new dafna_class();

print $name=call_user_func(array(&$dafna,"GetMyName"));
print "\n";

?>
--EXPECT--
0
Dafna!
I'm still alive
Hey there!!
0
1
Dafna!
I'm still alive
Hey there!!
1
2
Dafna!
I'm still alive
Hey there!!
2
3
Dafna!
I'm still alive
Hey there!!
3
4
Dafna!
I'm still alive
Hey there!!
4
5
Dafna!
I'm still alive
Hey there!!
5
6
Dafna!
I'm still alive
Hey there!!
6
7
Dafna!
I'm still alive
Hey there!!
7
8
Dafna!
I'm still alive
Hey there!!
8
9
Dafna!
I'm still alive
Hey there!!
9
10
Dafna!
I'm still alive
Hey there!!
10
11
Dafna!
I'm still alive
Hey there!!
11
12
Dafna!
I'm still alive
Hey there!!
12
13
Dafna!
I'm still alive
Hey there!!
13
14
Dafna!
I'm still alive
Hey there!!
14
15
Dafna!
I'm still alive
Hey there!!
15
16
Dafna!
I'm still alive
Hey there!!
16
17
Dafna!
I'm still alive
Hey there!!
17
18
Dafna!
I'm still alive
Hey there!!
18
19
Dafna!
I'm still alive
Hey there!!
19
20
Dafna!
I'm still alive
Hey there!!
20
21
Dafna!
I'm still alive
Hey there!!
21
22
Dafna!
I'm still alive
Hey there!!
22
23
Dafna!
I'm still alive
Hey there!!
23
24
Dafna!
I'm still alive
Hey there!!
24
25
Dafna!
I'm still alive
Hey there!!
25
26
Dafna!
I'm still alive
Hey there!!
26
27
Dafna!
I'm still alive
Hey there!!
27
28
Dafna!
I'm still alive
Hey there!!
28
29
Dafna!
I'm still alive
Hey there!!
29
30
Dafna!
I'm still alive
Hey there!!
30
31
Dafna!
I'm still alive
Hey there!!
31
32
Dafna!
I'm still alive
Hey there!!
32
33
Dafna!
I'm still alive
Hey there!!
33
34
Dafna!
I'm still alive
Hey there!!
34
35
Dafna!
I'm still alive
Hey there!!
35
36
Dafna!
I'm still alive
Hey there!!
36
37
Dafna!
I'm still alive
Hey there!!
37
38
Dafna!
I'm still alive
Hey there!!
38
39
Dafna!
I'm still alive
Hey there!!
39
40
Dafna!
I'm still alive
Hey there!!
40
41
Dafna!
I'm still alive
Hey there!!
41
42
Dafna!
I'm still alive
Hey there!!
42
43
Dafna!
I'm still alive
Hey there!!
43
44
Dafna!
I'm still alive
Hey there!!
44
45
Dafna!
I'm still alive
Hey there!!
45
46
Dafna!
I'm still alive
Hey there!!
46
47
Dafna!
I'm still alive
Hey there!!
47
48
Dafna!
I'm still alive
Hey there!!
48
49
Dafna!
I'm still alive
Hey there!!
49
50
Dafna!
I'm still alive
Hey there!!
50
51
Dafna!
I'm still alive
Hey there!!
51
52
Dafna!
I'm still alive
Hey there!!
52
53
Dafna!
I'm still alive
Hey there!!
53
54
Dafna!
I'm still alive
Hey there!!
54
55
Dafna!
I'm still alive
Hey there!!
55
56
Dafna!
I'm still alive
Hey there!!
56
57
Dafna!
I'm still alive
Hey there!!
57
58
Dafna!
I'm still alive
Hey there!!
58
59
Dafna!
I'm still alive
Hey there!!
59
60
Dafna!
I'm still alive
Hey there!!
60
61
Dafna!
I'm still alive
Hey there!!
61
62
Dafna!
I'm still alive
Hey there!!
62
63
Dafna!
I'm still alive
Hey there!!
63
64
Dafna!
I'm still alive
Hey there!!
64
65
Dafna!
I'm still alive
Hey there!!
65
66
Dafna!
I'm still alive
Hey there!!
66
67
Dafna!
I'm still alive
Hey there!!
67
68
Dafna!
I'm still alive
Hey there!!
68
69
Dafna!
I'm still alive
Hey there!!
69
70
Dafna!
I'm still alive
Hey there!!
70
71
Dafna!
I'm still alive
Hey there!!
71
72
Dafna!
I'm still alive
Hey there!!
72
73
Dafna!
I'm still alive
Hey there!!
73
74
Dafna!
I'm still alive
Hey there!!
74
75
Dafna!
I'm still alive
Hey there!!
75
76
Dafna!
I'm still alive
Hey there!!
76
77
Dafna!
I'm still alive
Hey there!!
77
78
Dafna!
I'm still alive
Hey there!!
78
79
Dafna!
I'm still alive
Hey there!!
79
80
Dafna!
I'm still alive
Hey there!!
80
81
Dafna!
I'm still alive
Hey there!!
81
82
Dafna!
I'm still alive
Hey there!!
82
83
Dafna!
I'm still alive
Hey there!!
83
84
Dafna!
I'm still alive
Hey there!!
84
85
Dafna!
I'm still alive
Hey there!!
85
86
Dafna!
I'm still alive
Hey there!!
86
87
Dafna!
I'm still alive
Hey there!!
87
88
Dafna!
I'm still alive
Hey there!!
88
89
Dafna!
I'm still alive
Hey there!!
89
90
Dafna!
I'm still alive
Hey there!!
90
91
Dafna!
I'm still alive
Hey there!!
91
92
Dafna!
I'm still alive
Hey there!!
92
93
Dafna!
I'm still alive
Hey there!!
93
94
Dafna!
I'm still alive
Hey there!!
94
95
Dafna!
I'm still alive
Hey there!!
95
96
Dafna!
I'm still alive
Hey there!!
96
97
Dafna!
I'm still alive
Hey there!!
97
98
Dafna!
I'm still alive
Hey there!!
98
99
Dafna!
I'm still alive
Hey there!!
99
100
Dafna!
I'm still alive
Hey there!!
100
101
Dafna!
I'm still alive
Hey there!!
101
102
Dafna!
I'm still alive
Hey there!!
102
103
Dafna!
I'm still alive
Hey there!!
103
104
Dafna!
I'm still alive
Hey there!!
104
105
Dafna!
I'm still alive
Hey there!!
105
106
Dafna!
I'm still alive
Hey there!!
106
107
Dafna!
I'm still alive
Hey there!!
107
108
Dafna!
I'm still alive
Hey there!!
108
109
Dafna!
I'm still alive
Hey there!!
109
110
Dafna!
I'm still alive
Hey there!!
110
111
Dafna!
I'm still alive
Hey there!!
111
112
Dafna!
I'm still alive
Hey there!!
112
113
Dafna!
I'm still alive
Hey there!!
113
114
Dafna!
I'm still alive
Hey there!!
114
115
Dafna!
I'm still alive
Hey there!!
115
116
Dafna!
I'm still alive
Hey there!!
116
117
Dafna!
I'm still alive
Hey there!!
117
118
Dafna!
I'm still alive
Hey there!!
118
119
Dafna!
I'm still alive
Hey there!!
119
120
Dafna!
I'm still alive
Hey there!!
120
121
Dafna!
I'm still alive
Hey there!!
121
122
Dafna!
I'm still alive
Hey there!!
122
123
Dafna!
I'm still alive
Hey there!!
123
124
Dafna!
I'm still alive
Hey there!!
124
125
Dafna!
I'm still alive
Hey there!!
125
126
Dafna!
I'm still alive
Hey there!!
126
127
Dafna!
I'm still alive
Hey there!!
127
128
Dafna!
I'm still alive
Hey there!!
128
129
Dafna!
I'm still alive
Hey there!!
129
130
Dafna!
I'm still alive
Hey there!!
130
131
Dafna!
I'm still alive
Hey there!!
131
132
Dafna!
I'm still alive
Hey there!!
132
133
Dafna!
I'm still alive
Hey there!!
133
134
Dafna!
I'm still alive
Hey there!!
134
135
Dafna!
I'm still alive
Hey there!!
135
136
Dafna!
I'm still alive
Hey there!!
136
137
Dafna!
I'm still alive
Hey there!!
137
138
Dafna!
I'm still alive
Hey there!!
138
139
Dafna!
I'm still alive
Hey there!!
139
140
Dafna!
I'm still alive
Hey there!!
140
141
Dafna!
I'm still alive
Hey there!!
141
142
Dafna!
I'm still alive
Hey there!!
142
143
Dafna!
I'm still alive
Hey there!!
143
144
Dafna!
I'm still alive
Hey there!!
144
145
Dafna!
I'm still alive
Hey there!!
145
146
Dafna!
I'm still alive
Hey there!!
146
147
Dafna!
I'm still alive
Hey there!!
147
148
Dafna!
I'm still alive
Hey there!!
148
149
Dafna!
I'm still alive
Hey there!!
149
150
Dafna!
I'm still alive
Hey there!!
150
151
Dafna!
I'm still alive
Hey there!!
151
152
Dafna!
I'm still alive
Hey there!!
152
153
Dafna!
I'm still alive
Hey there!!
153
154
Dafna!
I'm still alive
Hey there!!
154
155
Dafna!
I'm still alive
Hey there!!
155
156
Dafna!
I'm still alive
Hey there!!
156
157
Dafna!
I'm still alive
Hey there!!
157
158
Dafna!
I'm still alive
Hey there!!
158
159
Dafna!
I'm still alive
Hey there!!
159
160
Dafna!
I'm still alive
Hey there!!
160
161
Dafna!
I'm still alive
Hey there!!
161
162
Dafna!
I'm still alive
Hey there!!
162
163
Dafna!
I'm still alive
Hey there!!
163
164
Dafna!
I'm still alive
Hey there!!
164
165
Dafna!
I'm still alive
Hey there!!
165
166
Dafna!
I'm still alive
Hey there!!
166
167
Dafna!
I'm still alive
Hey there!!
167
168
Dafna!
I'm still alive
Hey there!!
168
169
Dafna!
I'm still alive
Hey there!!
169
170
Dafna!
I'm still alive
Hey there!!
170
171
Dafna!
I'm still alive
Hey there!!
171
172
Dafna!
I'm still alive
Hey there!!
172
173
Dafna!
I'm still alive
Hey there!!
173
174
Dafna!
I'm still alive
Hey there!!
174
175
Dafna!
I'm still alive
Hey there!!
175
176
Dafna!
I'm still alive
Hey there!!
176
177
Dafna!
I'm still alive
Hey there!!
177
178
Dafna!
I'm still alive
Hey there!!
178
179
Dafna!
I'm still alive
Hey there!!
179
180
Dafna!
I'm still alive
Hey there!!
180
181
Dafna!
I'm still alive
Hey there!!
181
182
Dafna!
I'm still alive
Hey there!!
182
183
Dafna!
I'm still alive
Hey there!!
183
184
Dafna!
I'm still alive
Hey there!!
184
185
Dafna!
I'm still alive
Hey there!!
185
186
Dafna!
I'm still alive
Hey there!!
186
187
Dafna!
I'm still alive
Hey there!!
187
188
Dafna!
I'm still alive
Hey there!!
188
189
Dafna!
I'm still alive
Hey there!!
189
190
Dafna!
I'm still alive
Hey there!!
190
191
Dafna!
I'm still alive
Hey there!!
191
192
Dafna!
I'm still alive
Hey there!!
192
193
Dafna!
I'm still alive
Hey there!!
193
194
Dafna!
I'm still alive
Hey there!!
194
195
Dafna!
I'm still alive
Hey there!!
195
196
Dafna!
I'm still alive
Hey there!!
196
197
Dafna!
I'm still alive
Hey there!!
197
198
Dafna!
I'm still alive
Hey there!!
198
199
Dafna!
I'm still alive
Hey there!!
199
Dafna
