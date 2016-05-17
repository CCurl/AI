// "$psi,$act,$hlc,$pos,$jux,$pre,$tkb,$seq,$num,$mfn,$dba,$rv";

// # PERSON-- for ad - hoc gender tags and robot philosophy
// $t = 561; $ear[$t] = "P,0,0";    # 2016feb08
// $t = 562; $ear[$t] = "E,0,0";    # 2016feb08
// $t = 563; $ear[$t] = "R,0,0";    # 2016feb08
// $t = 564; $ear[$t] = "S,0,0";    # 2016feb08
// $t = 565; $ear[$t] = "O,0,0";    # 2016feb08
// $t = 566; $ear[$t] = "N,0,537";  # 2016feb08

// $psi = 537; $hlc = "en"; $pos = 5; $jux = 0; $pre = 800; $tkb = 0; $seq = 0;
// $num = 1; $mfn = 0; $dba = 1; $rv = 561; KbLoad();  # 2016feb08

// our $act = 0;        # 2016jan16: quasi - neuronal activation - level
// our $audpsi = 0;     # 2016feb12: concept number of word in @ear array
// our $dba = 0;        # 2016jan16: doing-business-as noun-case or verb-person
// our $hlc = "en"		# human language construct
// our $jux = 0;        # 2016jan20: jux(taposed) concept in @psy array.
// our $pho = "";       # $pho is for a "phoneme" or character of input.
// our $pos = "5";		# 1=adj 2=adv 3=conj 4=interj 5=noun 6=prep 7=pron 8=verb
// our $mfn = 0;        # 2016jan16: masculine-feminine-neuter gender flag.
// our $num = 0;        # 2016jan16: number-flag for grammatical number.
// our $pre = 0;        # 2016jan20: pre(vious) associated @psy concept. 
// our $psi = 1;        # 2016jan19: variable for elements of @psy array
// our $rv = 0;         # 2016jan22: recall-vector for auditory memory. 
// our $seq = 0;        # 2016jan20: subSEQuent @psy concept in an idea
// our $tkb = 0;        # 2016jan20: time - in - knowledge - base of an idea.

// sub KbLoad() {
// # 2016jan23: load MindBoot knowledge - base bootstrap.
// 	if ($fyi > 2) {
// # 2016feb08: if mode is Diagnostic
// 		print "KbLoad stores MindBoot knowledge-base, t= $t. \n";  # 2016jan23
// 	}  # 2016feb07: end of test for Diagnostic or Tutorial mode.
// # https ://en.wikibooks.org/wiki/Perl_Programming/Unicode_UTF-8
// # encode("utf8", "$psi", 1);  # 2016feb16:
// 	$psy[$t] = "$psi,$act,$hlc,$pos,$jux,$pre,$tkb,$seq,$num,$mfn,$dba,$rv";
// }  # 2016jan23: End of KbLoad; return pToDendron MindBoot knowledge bootstrap.

// $audmem[$t] = "$pho,$act,$audpsi"
