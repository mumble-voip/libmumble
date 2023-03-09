// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_TESTBASE64_DATA_HPP
#define MUMBLE_TESTBASE64_DATA_HPP

#include <array>
#include <string_view>

struct Data {
	using Entry = std::pair< std::string_view, std::string_view >;
	using Table = std::array< Entry, 28 >;

	static constexpr Table ascii = {
		Entry("Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
			  "TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIGNvbnNlY3RldHVyIGFkaXBpc2NpbmcgZWxpdC4="),
		Entry("Nulla imperdiet est nec elit ornare dictum.",
			  "TnVsbGEgaW1wZXJkaWV0IGVzdCBuZWMgZWxpdCBvcm5hcmUgZGljdHVtLg=="),
		Entry("Integer elementum risus ac ipsum sodales sagittis.",
			  "SW50ZWdlciBlbGVtZW50dW0gcmlzdXMgYWMgaXBzdW0gc29kYWxlcyBzYWdpdHRpcy4="),
		Entry("Suspendisse id est id sapien elementum consequat ac sed magna.",
			  "U3VzcGVuZGlzc2UgaWQgZXN0IGlkIHNhcGllbiBlbGVtZW50dW0gY29uc2VxdWF0IGFjIHNlZCBtYWduYS4="),
		Entry("Praesent varius diam nec lorem semper elementum.",
			  "UHJhZXNlbnQgdmFyaXVzIGRpYW0gbmVjIGxvcmVtIHNlbXBlciBlbGVtZW50dW0u"),
		Entry("Suspendisse elementum dolor et placerat tincidunt.",
			  "U3VzcGVuZGlzc2UgZWxlbWVudHVtIGRvbG9yIGV0IHBsYWNlcmF0IHRpbmNpZHVudC4="),
		Entry("Duis accumsan metus sed augue rutrum feugiat.",
			  "RHVpcyBhY2N1bXNhbiBtZXR1cyBzZWQgYXVndWUgcnV0cnVtIGZldWdpYXQu"),
		Entry("Nam eu velit at massa fermentum rhoncus sed a diam.",
			  "TmFtIGV1IHZlbGl0IGF0IG1hc3NhIGZlcm1lbnR1bSByaG9uY3VzIHNlZCBhIGRpYW0u"),
		Entry("Nullam consectetur elit sit amet ante rutrum, eget consequat sem porttitor.",
			  "TnVsbGFtIGNvbnNlY3RldHVyIGVsaXQgc2l0IGFtZXQgYW50ZSBydXRydW0sIGVnZXQgY29uc2VxdWF0IHNlbSBwb3J0dGl0b3Iu"),
		Entry("Duis et ipsum mattis, tincidunt lacus sed, luctus risus.",
			  "RHVpcyBldCBpcHN1bSBtYXR0aXMsIHRpbmNpZHVudCBsYWN1cyBzZWQsIGx1Y3R1cyByaXN1cy4="),
		Entry("In convallis dui nec nisi pellentesque, ac maximus massa posuere.",
			  "SW4gY29udmFsbGlzIGR1aSBuZWMgbmlzaSBwZWxsZW50ZXNxdWUsIGFjIG1heGltdXMgbWFzc2EgcG9zdWVyZS4="),
		Entry("Curabitur quis mi tincidunt, maximus orci non, semper arcu.",
			  "Q3VyYWJpdHVyIHF1aXMgbWkgdGluY2lkdW50LCBtYXhpbXVzIG9yY2kgbm9uLCBzZW1wZXIgYXJjdS4="),
		Entry("Pellentesque ac ex placerat massa tincidunt efficitur.",
			  "UGVsbGVudGVzcXVlIGFjIGV4IHBsYWNlcmF0IG1hc3NhIHRpbmNpZHVudCBlZmZpY2l0dXIu"),
		Entry("Suspendisse quis neque ac neque suscipit fermentum et in ligula.",
			  "U3VzcGVuZGlzc2UgcXVpcyBuZXF1ZSBhYyBuZXF1ZSBzdXNjaXBpdCBmZXJtZW50dW0gZXQgaW4gbGlndWxhLg=="),
		Entry("Morbi in lacus et augue dapibus porttitor.", "TW9yYmkgaW4gbGFjdXMgZXQgYXVndWUgZGFwaWJ1cyBwb3J0dGl0b3Iu"),
		Entry("Vestibulum tincidunt erat quis feugiat imperdiet.",
			  "VmVzdGlidWx1bSB0aW5jaWR1bnQgZXJhdCBxdWlzIGZldWdpYXQgaW1wZXJkaWV0Lg=="),
		Entry("Donec auctor felis non urna congue, at pretium leo aliquam.",
			  "RG9uZWMgYXVjdG9yIGZlbGlzIG5vbiB1cm5hIGNvbmd1ZSwgYXQgcHJldGl1bSBsZW8gYWxpcXVhbS4="),
		Entry("In sollicitudin neque id eros tristique posuere.",
			  "SW4gc29sbGljaXR1ZGluIG5lcXVlIGlkIGVyb3MgdHJpc3RpcXVlIHBvc3VlcmUu"),
		Entry("Mauris mattis erat at velit tempus bibendum.",
			  "TWF1cmlzIG1hdHRpcyBlcmF0IGF0IHZlbGl0IHRlbXB1cyBiaWJlbmR1bS4="),
		Entry("Duis tempus lectus eu turpis pulvinar, luctus maximus libero hendrerit.",
			  "RHVpcyB0ZW1wdXMgbGVjdHVzIGV1IHR1cnBpcyBwdWx2aW5hciwgbHVjdHVzIG1heGltdXMgbGliZXJvIGhlbmRyZXJpdC4="),
		Entry("Nam sodales nibh sed tellus eleifend consectetur.",
			  "TmFtIHNvZGFsZXMgbmliaCBzZWQgdGVsbHVzIGVsZWlmZW5kIGNvbnNlY3RldHVyLg=="),
		Entry("Ut elementum lorem eu pulvinar fringilla.", "VXQgZWxlbWVudHVtIGxvcmVtIGV1IHB1bHZpbmFyIGZyaW5naWxsYS4="),
		Entry("Phasellus non diam a odio accumsan suscipit eu vulputate lacus.",
			  "UGhhc2VsbHVzIG5vbiBkaWFtIGEgb2RpbyBhY2N1bXNhbiBzdXNjaXBpdCBldSB2dWxwdXRhdGUgbGFjdXMu"),
		Entry("In posuere est a nisi scelerisque, sed bibendum nibh gravida.",
			  "SW4gcG9zdWVyZSBlc3QgYSBuaXNpIHNjZWxlcmlzcXVlLCBzZWQgYmliZW5kdW0gbmliaCBncmF2aWRhLg=="),
		Entry("Integer imperdiet nunc vitae orci porta, ac facilisis justo porttitor.",
			  "SW50ZWdlciBpbXBlcmRpZXQgbnVuYyB2aXRhZSBvcmNpIHBvcnRhLCBhYyBmYWNpbGlzaXMganVzdG8gcG9ydHRpdG9yLg=="),
		Entry("Aenean egestas arcu ac sapien efficitur porttitor.",
			  "QWVuZWFuIGVnZXN0YXMgYXJjdSBhYyBzYXBpZW4gZWZmaWNpdHVyIHBvcnR0aXRvci4="),
		Entry("Quisque quis erat sit amet nunc tincidunt vestibulum.",
			  "UXVpc3F1ZSBxdWlzIGVyYXQgc2l0IGFtZXQgbnVuYyB0aW5jaWR1bnQgdmVzdGlidWx1bS4="),
		Entry("Mauris ac dui ac elit ullamcorper consectetur.",
			  "TWF1cmlzIGFjIGR1aSBhYyBlbGl0IHVsbGFtY29ycGVyIGNvbnNlY3RldHVyLg==")
	};

	static constexpr Table unicode = {
		Entry(
			"⑩⇁≒⼟⏟⚰⏚ⸯ⑓⅕⋬⡠⭅ⴂ⻠ℿ∮ⴷ⑶⅝⵺⛺⌇‹≔⻿ⵁ☶ⱚ≼⍿ⰹ⩎⮴⾉"
			"⧣"
			"⤁"
			"◐"
			"⊯"
			"⊲"
			"⺄"
			"⤥"
			"⛃"
			"⇔"
			"⚻"
			"⽊"
			"⩤"
			"⌁"
			"⢕"
			"Ⱈ"
			"ⱙ"
			"⪺"
			"❤"
			"⽶"
			"⒖"
			"⡯",
			"4pGp4oeB4omS4ryf4o+f4pqw4o+a4riv4pGT4oWV4ous4qGg4q2F4rSC4rug4oS/4oiu4rS34pG24oWd4rW64pu64oyH4oC54omU4ru/"
			"4rWB4pi24rGa4om84o2/"
			"4rC54qmO4q604r6J4qej4qSB4peQ4oqv4oqy4rqE4qSl4puD4oeU4pq74r2K4qmk4oyB4qKV4rCY4rGZ4qq64p2k4r224pKW4qGv"),
		Entry(
			"Ⳙ⌯⑰Ⳃ♝≳⦒ⵓ⠦⎦₄⨋⤖⼅∻ℒ♻Ⱞⅶ⏿ⓙ⊭⹦ⴴⵝⰅ⎄Å⢅⭺⼙⾹☂ⵣ⮘"
			"₆"
			"☀"
			"⁻"
			"⨙"
			"ⱼ"
			"⯕"
			"⹋"
			"ⶄ",
			"4rOY4oyv4pGw4rOC4pmd4omz4qaS4rWT4qCm4o6m4oKE4qiL4qSW4ryF4oi74oSS4pm74rCu4oW24o+/"
			"4pOZ4oqt4rmm4rS04rWd4rCF4o6E4oSr4qKF4q264ryZ4r654piC4rWj4q6Y4oKG4piA4oG74qiZ4rG84q+V4rmL4raE"),
		Entry(
			"⬏⽖ⶂ◨⒣⸁⮄⍵ⷵ⌾⑆⠕⌌⁫⏨⼵⎺⎅⮂ⶈ❾ⅸ⻍⽕⮀⚓∌ⲍ∁▼☿⻑╈◳↍"
			"⌷"
			"⛀"
			"ⴑ"
			"⎞"
			"⛏"
			"╡"
			"♾"
			"⹰"
			"Ⅎ"
			"⤕"
			"⡸"
			"⻓"
			"⸍"
			"⒯"
			"⣐",
			"4qyP4r2W4raC4peo4pKj4riB4q6E4o214re14oy+4pGG4qCV4oyM4oGr4o+o4ry14o664o6F4q6C4raI4p2+"
			"4oW44ruN4r2V4q6A4pqT4oiM4rKN4oiB4pa84pi/"
			"4ruR4pWI4pez4oaN4oy34puA4rSR4o6e4puP4pWh4pm+4rmw4oSy4qSV4qG44ruT4riN4pKv4qOQ"),
		Entry(
			"⿶ ✘⇃◾↼⡧⽀Ⰵ⡩␨⽮⦙⤖⮪ↈ⣝⟝℥✬⺓ⓥ⺡┶⻋┩⧊ⰽ➡⥃↛⽙⬉②⌹"
			"⚐"
			"⳾"
			"♖"
			"⌣"
			"ⳣ"
			"⪂"
			"Ⱚ"
			"∅"
			"▇"
			"✄"
			"⩣"
			"Ⲿ"
			"⹝"
			"⊅"
			"⊆"
			"⇽"
			"⻉"
			"⤰"
			"☒"
			"⋮"
			"⡿"
			"␣"
			"⠈"
			"⍅"
			"⨥"
			"⣒"
			"⮄",
			"4r+24oCD4pyY4oeD4pe+4oa84qGn4r2A4rCF4qGp4pCo4r2u4qaZ4qSW4q6q4oaI4qOd4p+"
			"d4oSl4pys4rqT4pOl4rqh4pS24ruL4pSp4qeK4rC94p6h4qWD4oab4r2Z4qyJ4pGh4oy54pqQ4rO+"
			"4pmW4oyj4rOj4qqC4rCq4oiF4paH4pyE4qmj4rK+4rmd4oqF4oqG4oe94ruJ4qSw4piS4ouu4qG/4pCj4qCI4o2F4qil4qOS4q6E"),
		Entry(
			"Ⓦ♓⚀⏁ℒⓄⱷ₋℄⎘⹥⫏⧣ⷣ⟎♹⹹⍍✒⠷␶⫁⦼⚤⼺⡅⑍⍿∨⭸ⅉ⹹⤃⸥⧼"
			"⃎"
			"⚽"
			"⑉"
			"⤧"
			"∼"
			"⫛"
			"ℕ"
			"⦉"
			"‥"
			"ⅼ"
			"⟶"
			"⯔"
			"⨔",
			"4pOM4pmT4pqA4o+B4oSS4pOE4rG34oKL4oSE4o6Y4rml4quP4qej4rej4p+"
			"O4pm54rm54o2N4pyS4qC34pC24quB4qa84pqk4ry64qGF4pGN4o2/"
			"4oio4q244oWJ4rm54qSD4ril4qe84oOO4pq94pGJ4qSn4oi84qub4oSV4qaJ4oCl4oW84p+24q+U4qiU"),
		Entry(
			"⠠⢡↡⳿∈ⳝ⒘⧦⡁┄⚝⦎⬻ⶼ‶⦮ⲣ⦹♨△ₙ⁲ⷀ⌱⅟⡫⿉⮿␠⇉⋏⤃ⱎ≽ⓖ"
			"∕"
			"⻢"
			"⤊"
			"⏱"
			"ℎ"
			"Ⲻ"
			"ℎ"
			"⛓"
			"▃"
			"↯"
			"⃫"
			"⦽"
			"╅"
			"⺄"
			"Ⱔ",
			"4qCg4qKh4oah4rO/"
			"4oiI4rOd4pKY4qem4qGB4pSE4pqd4qaO4qy74ra84oC24qau4rKj4qa54pmo4paz4oKZ4oGy4reA4oyx4oWf4qGr4r+J4q6/"
			"4pCg4oeJ4ouP4qSD4rGO4om94pOW4oiV4rui4qSK4o+x4oSO4rK64oSO4puT4paD4oav4oOr4qa94pWF4rqE4rCk"),
		Entry(
			"⼚❣➓⨹⮺⌆⫹⠔⅍⑼⨡∴➃⤸ℸⵦ⁤⪈◼⡬⳸⥯⾜⿜ⱼ⃒⬄⭷⊝⅛⹺╱⤲⦖≀"
			"⋝"
			"⟓"
			"⸛"
			"⥃"
			"⺽"
			"⇀"
			"⠨"
			"⻖"
			"⌕"
			"✑",
			"4rya4p2j4p6T4qi54q664oyG4qu54qCU4oWN4pG84qih4oi04p6D4qS44oS44rWm4oGk4qqI4pe84qGs4rO44qWv4r6c4r+"
			"c4rG84oOS4qyE4q234oqd4oWb4rm64pWx4qSy4qaW4omA4oud4p+T4rib4qWD4rq94oeA4qCo4ruW4oyV4pyR"),
		Entry(
			"ⶳ⬳⯭➅⒜⯴∕⸇⡣⣿⼣⍱◲⾣₻␍⒘℄⡎⸒⼞⊗⦖☣✯☣ⴟⰶ⶷⟕⠯℣⼎⢒Ⰿ"
			"⼐"
			"⻸"
			"⭅"
			"Ⅽ"
			"⹖"
			"℟"
			"ⱖ"
			"Ⱝ"
			"╱"
			"‽"
			"⫋"
			"⇝"
			"⸐"
			"⤜"
			"↡"
			"⓾",
			"4raz4qyz4q+t4p6F4pKc4q+04oiV4riH4qGj4qO/"
			"4ryj4o2x4pey4r6j4oK74pCN4pKY4oSE4qGO4riS4rye4oqX4qaW4pij4pyv4pij4rSf4rC24ra34p+"
			"V4qCv4oSj4ryO4qKS4rCP4ryQ4ru44q2F4oWt4rmW4oSf4rGW4rCt4pWx4oC94quL4oed4riQ4qSc4oah4pO+"),
		Entry(
			"⟼⃚⶛ⲁ⽁⋞⿏∐◃⡖❨ⴸⲚ⚲ⷰ⨑␋⿓⒍⣔⾒♸⋆⬘⛌≼ⰹ➲⴪⮭⣯⽷⊺Ⰲⶨ"
			"⺃"
			"⣛"
			"ⷒ"
			"↬"
			"⿰"
			"⾺"
			"⍀"
			"⼒"
			"₥"
			"⤯"
			"⼩"
			"⬌"
			"⚽"
			"⌫"
			"⁸"
			"Ⰿ"
			"⫢"
			"ℋ"
			"⤐"
			"⡵"
			"⇕"
			"⼛"
			"⻘"
			"ⷘ"
			"Ⱳ"
			"ⶏ"
			"⒌"
			"⪓"
			"⪫"
			"␙"
			"◗"
			"ⶒ"
			"⢺"
			"•"
			"₊"
			"⊆"
			"⦇"
			"⏃"
			"⍊"
			"⍂",
			"4p+84oOa4rab4rKB4r2B4oue4r+P4oiQ4peD4qGW4p2o4rS44rKa4pqy4rew4qiR4pCL4r+"
			"T4pKN4qOU4r6S4pm44ouG4qyY4puM4om84rC54p6y4rSq4q6t4qOv4r234oq64rCC4rao4rqD4qOb4reS4oas4r+"
			"w4r664o2A4ryS4oKl4qSv4ryp4qyM4pq94oyr4oG44rCP4qui4oSL4qSQ4qG14oeV4ryb4ruY4reY4rGy4raP4pKM4qqT4qqr4pCZ4pe"
			"X4raS4qK64oCi4oKK4oqG4qaH4o+D4o2K4o2C"),
		Entry(
			"↤≢≞⣴⅑⢛⍱⾷⽠ⷋ⦖⑻┨⎈❅⒒☀ⴀ∿⿚⧡⏪Ⓓ⚆Ⱟ⋽⊥ⷦ⇂⫙⼄⊟⻸♯☶"
			"⭩"
			"₩"
			"Ⲿ"
			"⑪"
			"⃜"
			"⪼"
			"⤼"
			"Ȿ"
			"➄"
			"☳"
			"⏫"
			"⡾"
			"⡙"
			"⧨"
			"⇣"
			"⧪"
			"⹴"
			"⩋"
			"⤇"
			"␅"
			"Ⅱ",
			"4oak4omi4ome4qO04oWR4qKb4o2x4r634r2g4reL4qaW4pG74pSo4o6I4p2F4pKS4piA4rSA4oi/"
			"4r+a4qeh4o+q4pK54pqG4rCv4ou94oql4rem4oeC4quZ4ryE4oqf4ru44pmv4pi24q2p4oKp4rK+4pGq4oOc4qq84qS84rG+"
			"4p6E4piz4o+r4qG+4qGZ4qeo4oej4qeq4rm04qmL4qSH4pCF4oWh"),
		Entry(
			"⮾✦↬◄♣↼⧢⟕⥷⠆⻡ⷃ⊼┛⁳⻔␟⩱⺡⦦₨⹏✕⏃₌∲⿇⤘⿇ⵠ⡊⏥❊⮍⽺"
			"⟰"
			"⽨"
			"▻"
			"⯍"
			"⩽"
			"❕"
			"ⷑ"
			"∱"
			"⤷"
			"↬"
			"ⴗ"
			"◰"
			"ⲱ"
			"⯯"
			"⌄"
			"Ⱋ"
			"Ⲏ"
			"⢲"
			"⟯"
			"✬"
			"╥"
			"␏"
			"Ⱑ"
			"⬠"
			"⸓"
			"₞"
			"➖"
			"⌳"
			"⮒"
			"╟",
			"4q6+4pym4oas4peE4pmj4oa84qei4p+V4qW34qCG4ruh4reD4oq84pSb4oGz4ruU4pCf4qmx4rqh4qam4oKo4rmP4pyV4o+"
			"D4oKM4oiy4r+H4qSY4r+H4rWg4qGK4o+l4p2K4q6N4r264p+w4r2o4pa74q+N4qm94p2V4reR4oix4qS34oas4rSX4pew4rKx4q+"
			"v4oyE4rCb4rKO4qKy4p+v4pys4pWl4pCP4rCh4qyg4riT4oKe4p6W4oyz4q6S4pWf"),
		Entry(
			"▥☴⵱⯦⁳❉Ⅽ⳾⎡⻏⬍⮊⣪‏∷⿡⩫⁍⁡▰ⵙ⿁⭌ⷌ⹱⃅⛻⴨⯉ⱪ␑⦴⹠⓻⡢"
			"⏮"
			"⦀"
			"␴"
			"⪃"
			"⢖"
			"⬙"
			"❖"
			"☖"
			"⬙"
			"⣧"
			"⛄"
			"⣎"
			"┿"
			"≴"
			"⤖"
			"⽅"
			"⇛"
			"ⱳ"
			"⨰"
			"♀"
			"⋇"
			"⠜"
			"⯩"
			"ⓒ",
			"4pal4pi04rWx4q+m4oGz4p2J4oWt4rO+4o6h4ruP4qyN4q6K4qOq4oCP4oi34r+h4qmr4oGN4oGh4paw4rWZ4r+"
			"B4q2M4reM4rmx4oOF4pu74rSo4q+J4rGq4pCR4qa04rmg4pO74qGi4o+"
			"u4qaA4pC04qqD4qKW4qyZ4p2W4piW4qyZ4qOn4puE4qOO4pS/4om04qSW4r2F4oeb4rGz4qiw4pmA4ouH4qCc4q+p4pOS"),
		Entry(
			"⭌␔ⱜ⑛⭜▜⃥⥑⇍⢶┻⿨⎂⋫⑆◖⻻ ≷◹⩴⌹⠴▂⽻⒝⡃ⓢⰺ⫡✸⼼⃞ⅶ⹻"
			"⳵"
			"⹢"
			"⦾"
			"⭪"
			"⒋"
			"⁘"
			"ℙ"
			"⒦"
			"≪"
			"ⱸ"
			"ℰ"
			"≗"
			"⺀"
			"Ⱉ"
			"⾚"
			"␞"
			"⵸"
			"⾄"
			"⫖",
			"4q2M4pCU4rGc4pGb4q2c4pac4oOl4qWR4oeN4qK24pS74r+"
			"o4o6C4our4pGG4peW4ru74oCI4om34pe54qm04oy54qC04paC4r274pKd4qGD4pOi4rC64quh4py44ry84oOe4oW24rm74rO14rmi4qa"
			"+4q2q4pKL4oGY4oSZ4pKm4omq4rG44oSw4omX4rqA4rCZ4r6a4pCe4rW44r6E4quW"),
		Entry("⩽⑥⧡⊌⎄≜ⓓⵘ⩭Ⲭ⫟␍⤍♍⨝⪡⚛⦭ⅾ⾖⨏❗⦍⢗⍅ⰰ‰ⲭℋⴜ┗ⷪ⍄⇑ↀ◡⒋◚⅝⃐ⶦ⪋∮⣎ℂ➓"
			  "⿞"
			  "⁼"
			  "⼰"
			  "⽽"
			  "ⶅ"
			  "⨹"
			  "ⱇ"
			  "⤻"
			  "♣"
			  "↰"
			  "␌"
			  "⌾"
			  "⦜"
			  "⌙"
			  "╹"
			  "⤅"
			  "⠑"
			  "✽",
			  "4qm94pGl4qeh4oqM4o6E4omc4pOT4rWY4qmt4rKs4quf4pCN4qSN4pmN4qid4qqh4pqb4qat4oW+"
			  "4r6W4qiP4p2X4qaN4qKX4o2F4rCw4oCw4rKt4oSL4rSc4pSX4req4o2E4oeR4oaA4peh4pKL4pea4oWd4oOQ4ram4qqL4oiu4qOO4oSC"
			  "4p6T4r+e4oG84ryw4r294raF4qi54rGH4qS74pmj4oaw4pCM4oy+4qac4oyZ4pW54qSF4qCR4py9"),
		Entry(
			"⣹⪰❅ⰲ♭➽Ⓢ✈⌽⟞◤ⵙ⫧ⴵ♇★⯗⻨ⲧ▖⭮◄⬭⮼⎼␩⢷⽑ⵈ⥆❧⇼␝⺭⊰"
			"⠟"
			"✺"
			"⸋"
			"⃢"
			"❤"
			"ⴹ"
			"⅙",
			"4qO54qqw4p2F4rCy4pmt4p694pOI4pyI4oy94p+e4pek4rWZ4qun4rS14pmH4piF4q+"
			"X4ruo4rKn4paW4q2u4peE4qyt4q684o684pCp4qK34r2R4rWI4qWG4p2n4oe84pCd4rqt4oqw4qCf4py64riL4oOi4p2k4rS54oWZ"),
		Entry(
			"⃶⚈⧐⭬⥵⒨⦉⦩⋵⃒⛷❢⯭℟⎜╳ⳣ⹑⧘⡂ⅸ⚴⋧⦁Ⓦ⤻↞∼⥒⦂⃛⺁ⱊ⮓▃"
			"⮧"
			"℈"
			"◔"
			"⢲"
			"⨌"
			"⨪"
			"⣟"
			"⚂"
			"⩑"
			"⼃"
			"▵"
			"♉"
			"⫗"
			"❌",
			"4oO24pqI4qeQ4q2s4qW14pKo4qaJ4qap4ou14oOS4pu34p2i4q+"
			"t4oSf4o6c4pWz4rOj4rmR4qeY4qGC4oW44pq04oun4qaB4pOM4qS74oae4oi84qWS4qaC4oOb4rqB4rGK4q6T4paD4q6n4oSI4peU4qK"
			"y4qiM4qiq4qOf4pqC4qmR4ryD4pa14pmJ4quX4p2M"),
		Entry(
			"⧰⚓⭺⽩➞⍂⯏⥧⽚⇜⧇⯬➬ℵ‘✛⿕♺Ⱗ♋➴▟≂⬛⭳ⵋⷭ╫⳱⫻ⷪ⒪⩗≢⸬"
			"ⳑ"
			"␩"
			"⹼"
			"Ⅾ"
			"⾄"
			"➣"
			"Ⱁ"
			"⤵"
			"⡗"
			"☊"
			"ⅲ"
			"ⓦ"
			"✟"
			"⊬"
			"⻳"
			"⏳"
			"⍀"
			"⃕"
			"✏"
			"ⷵ"
			"⡣"
			"❰"
			"↨"
			"▁",
			"4qew4pqT4q264r2p4p6e4o2C4q+P4qWn4r2a4oec4qeH4q+s4p6s4oS14oCY4pyb4r+"
			"V4pm64rCn4pmL4p604paf4omC4qyb4q2z4rWL4ret4pWr4rOx4qu74req4pKq4qmX4omi4ris4rOR4pCp4rm84oWu4r6E4p6j4rCR4qS"
			"14qGX4piK4oWy4pOm4pyf4oqs4ruz4o+z4o2A4oOV4pyP4re14qGj4p2w4oao4paB"),
		Entry(
			"⯽␺⫥⸆⥟⎌ⱺ⇺◠⟛⊨ⴓ⽘ℓ≍⒧ⴉ⇴♬⥏⟉⪦⒚⒵⣖Ⲳ⣫☨⤭╃⣷‵∗ↄ╳"
			"❁"
			"⟺"
			"⻕"
			"␺"
			"⦼"
			"⽢"
			"ⱝ"
			"⿟"
			"⎖"
			"ⵝ"
			"‸"
			"⽠"
			"⎀",
			"4q+94pC64qul4riG4qWf4o6M4rG64oe64peg4p+b4oqo4rST4r2Y4oST4omN4pKn4rSJ4oe04pms4qWP4p+"
			"J4qqm4pKa4pK14qOW4rKy4qOr4pio4qSt4pWD4qO34oC14oiX4oaE4pWz4p2B4p+64ruV4pC64qa84r2i4rGd4r+"
			"f4o6W4rWd4oC44r2g4o6A"),
		Entry(
			"♱➩⧣⼰⑘⣉ⲓ⁓⧂₤▿⟽⒒⪺ⰺ⠰⫞▏℀⛯⇟Ⱔ◵⬡≬ⴞ⸩☇≉⤒⊵⨍␳⮭⃣"
			"ℽ"
			"⌤"
			"⒅"
			"Ⳕ"
			"⽺"
			"⭚"
			"⽶"
			"⦦"
			"⥁",
			"4pmx4p6p4qej4ryw4pGY4qOJ4rKT4oGT4qeC4oKk4pa/"
			"4p+"
			"94pKS4qq64rC64qCw4que4paP4oSA4puv4oef4rCk4pe14qyh4oms4rSe4rip4piH4omJ4qSS4oq14qiN4pCz4q6t4oOj4oS94oyk4pK"
			"F4rOU4r264q2a4r224qam4qWB"),
		Entry(
			"⻳⤡⻧⦶Ⰱ➗⼮⹀⬹☌ⲷ⾑⟛⛟⺇∾Ɑ⍱₄⍇⓱♑⮰◚≱⇣⍮⷗ ⴘₐ♬⟌⮳⋪"
			"⿍"
			"∎"
			"⃝"
			"⺸"
			"⠳"
			"ⱍ"
			"✙"
			"⋎"
			"⚆"
			"⯴"
			"⥓"
			"⫭"
			"∗"
			"⏒"
			"⃠"
			"↖"
			"₦"
			"⩩"
			"⥭"
			"◩"
			"⣸"
			"⁍"
			"⻿"
			"Ⱙ"
			"⸐"
			"ⓦ"
			"⧓"
			"ⅇ"
			"⧛"
			"⸓"
			"⋊"
			"⮆"
			"⁞"
			"╫"
			"ⳍ"
			"⧞",
			"4ruz4qSh4run4qa24rCB4p6X4ryu4rmA4qy54piM4rK34r6R4p+b4puf4rqH4oi+"
			"4rGt4o2x4oKE4o2H4pOx4pmR4q6w4pea4omx4oej4o2u4reX4oCv4rSY4oKQ4pms4p+M4q6z4ouq4r+"
			"N4oiO4oOd4rq44qCz4rGN4pyZ4ouO4pqG4q+04qWT4qut4oiX4o+S4oOg4oaW4oKm4qmp4qWt4pep4qO44oGN4ru/"
			"4rCp4riQ4pOm4qeT4oWH4qeb4riT4ouK4q6G4oGe4pWr4rON4qee"),
		Entry(
			"┰⌐Ⓩⳬ ⡣ⴟ⟢↚⡛☂⪙⾣☯⿸∫ⵟ☲ⵃ⦺⧘⺷⯹∔Ⓛ⹂⦓⪂➥⃅⯾⃦⺡₫⛕"
			"⒅"
			"❆"
			"Ⱘ"
			"┢"
			"⺮"
			"⤰"
			"ⲍ"
			"ⴰ"
			"ⶂ"
			"⭲"
			"ⶶ"
			"⁯"
			"⡾"
			"⢥",
			"4pSw4oyQ4pOP4rOs4oCD4qGj4rSf4p+i4oaa4qGb4piC4qqZ4r6j4piv4r+44oir4rWf4piy4rWD4qa64qeY4rq34q+"
			"54oiU4pOB4rmC4qaT4qqC4p6l4oOF4q++4oOm4rqh4oKr4puV4pKF4p2G4rCo4pSi4rqu4qSw4rKN4rSw4raC4q2y4ra24oGv4qG+"
			"4qKl"),
		Entry(
			"ℙ⍴ℍⵄ⺟⁹⣟⏵ⷴ⎗⾏⨖♙⥠⢍⦰ₛ₴ ⍸␶⍁⏓⛗≄⎉⮦ⲧ⢭⢩⓭∎⃨⾗ⱡ"
			"≂"
			"慌"
			"∳"
			"Ᵽ"
			"╚"
			"⚮",
			"4oSZ4o204oSN4rWE4rqf4oG54qOf4o+14re04o6X4r6P4qiW4pmZ4qWg4qKN4qaw4oKb4oK04oCK4o244pC24o2B4o+"
			"T4puX4omE4o6J4q6m4rKn4qKt4qKp4pOt4oiO4oOo4r6X4rGh4omC5oWM4oiz4rGj4pWa4pqu"),
		Entry(
			"␰∷➄Ⱒ⍝⠭⩁⨋⼀⡥⨪⅕∃↽※⁔⋴◌ⰶ⥝ⶩ⿐⛖⎛ⱅ⣉⇸ⶲ〉⚟Ɑ⨸❪⯵⭃"
			"⌘"
			"⅞"
			"ₜ"
			"⣎"
			"⦿"
			"⑋"
			"⩩"
			"✱"
			"⛕"
			"⁸"
			"⃶"
			"ⷁ"
			"․"
			"⮱"
			"⪁"
			"⎌"
			"⤀"
			"╥"
			"⯌"
			"⺡"
			"⛳"
			"⊖"
			"⁝"
			"瀍"
			"★"
			"ℝ"
			"☢"
			"⚍",
			"4pCw4oi34p6E4rCi4o2d4qCt4qmB4qiL4ryA4qGl4qiq4oWV4oiD4oa94oC74oGU4ou04peM4rC24qWd4rap4r+"
			"Q4puW4o6b4rGF4qOJ4oe44ray4oyq4pqf4rGt4qi44p2q4q+14q2D4oyY4oWe4oKc4qOO4qa/"
			"4pGL4qmp4pyx4puV4oG44oO24reB4oCk4q6x4qqB4o6M4qSA4pWl4q+M4rqh4puz4oqW4oGd54CN4piF4oSd4pii4pqN"),
		Entry(
			"≪␯⿆⭕☎⟞⎪⢇⊤⊾⎦℄⃙⧛ⓠⲅ❬⃸◣▞⚓⏱ⵘ✡Ⲓ⅏⊮⺋✫ⱏ♹⩺⭿⢡➎"
			"⨲"
			"⿑"
			"⸈"
			"Ⅻ"
			"⟖"
			"ⴚ"
			"⿈"
			"⫡"
			"ⱜ"
			"⅟"
			"⾑"
			"⭴"
			"ℌ"
			"⭡"
			"⧮"
			"⁾"
			"⮍"
			"⊨"
			"⯶"
			"₏"
			"⼏"
			"⧞"
			"⒫"
			"⢵"
			"⏂"
			"◕",
			"4omq4pCv4r+G4q2V4piO4p+e4o6q4qKH4oqk4oq+4o6m4oSE4oOZ4qeb4pOg4rKF4p2s4oO44pej4pae4pqT4o+"
			"x4rWY4pyh4rKS4oWP4oqu4rqL4pyr4rGP4pm54qm64q2/"
			"4qKh4p6O4qiy4r+R4riI4oWr4p+W4rSa4r+I4quh4rGc4oWf4r6R4q204oSM4q2h4qeu4oG+4q6N4oqo4q+"
			"24oKP4ryP4qee4pKr4qK14o+C4peV"),
		Entry(
			"♇⯘⁯⏫ⴐ⚁␇⼕ⴛₜ⟿ⶥⓩ☁⛱⇍⁛⯮⓾⼫⇳╺⼸​⑤⢶⑂⭜ⲣ⍆Ⰿ╆≄∗⇯"
			"⼱"
			"ⷬ"
			"◾"
			"⨵"
			"Ⱜ"
			"◠"
			"◡"
			"⊃"
			"␐"
			"❾"
			"⎨"
			"⺺"
			"⡻"
			"⑔"
			"⽶"
			"⏁"
			"⟫"
			"ⶾ"
			"∿"
			"⚩"
			"ⰷ"
			"⍱"
			"⢛"
			"⊊"
			"⮇"
			"⍚"
			"⎠"
			"♇"
			"⽝"
			"⚂"
			"⧄"
			"⸕"
			"⹙"
			"⡔"
			"⽇",
			"4pmH4q+Y4oGv4o+r4rSQ4pqB4pCH4ryV4rSb4oKc4p+/"
			"4ral4pOp4piB4pux4oeN4oGb4q+u4pO+"
			"4ryr4oez4pW64ry44oCL4pGk4qK24pGC4q2c4rKj4o2G4rCP4pWG4omE4oiX4oev4ryx4res4pe+4qi14rCs4peg4peh4oqD4pCQ4p2+"
			"4o6o4rq64qG74pGU4r224o+B4p+r4ra+4oi/4pqp4rC34o2x4qKb4oqK4q6H4o2a4o6g4pmH4r2d4pqC4qeE4riV4rmZ4qGU4r2H"),
		Entry(
			"☉⨐⯫⺌⋬⣴ⲅ⇣⧈⦞⎡↫⪭⻷⥫⻆⅘Ⰿ⊻⑳ⓧ⸦◲⺋⻣ⴚ⮭☍⠕▮␮⟆⺚⢈⍮"
			"⿣"
			"⼰"
			"⹵"
			"⡕"
			"ⴭ"
			"⦈"
			"⳺"
			"⽂"
			"≥"
			"♼"
			"⺺"
			"Ⲟ"
			"ⅅ"
			"‽"
			"⵺",
			"4piJ4qiQ4q+"
			"r4rqM4ous4qO04rKF4oej4qeI4qae4o6h4oar4qqt4ru34qWr4ruG4oWY4rCP4oq74pGz4pOn4rim4pey4rqL4ruj4rSa4q6t4piN4qC"
			"V4pau4pCu4p+G4rqa4qKI4o2u4r+j4ryw4rm14qGV4rSt4qaI4rO64r2C4oml4pm84rq64rKe4oWF4oC94rW6"),
		Entry(
			"⨭⃿☞◮╡⮼✂⭉⚧⃶⫕♬⻒⇭⠥⏢ⳡⅽ↜⏁Ⰲ⯅⒥┒⑄⇤⎡ⷱ⭎⁓⃿⛍‼⼻☧"
			"ℛ"
			"⎦"
			"⧚"
			"⢱"
			"⼳"
			"⋥"
			"⻮"
			"⍩"
			"⛝"
			"⾲"
			"⇵"
			"Ⲫ"
			"⎤"
			"⳻"
			"ⴍ"
			"⣆"
			"∸"
			"↷",
			"4qit4oO/"
			"4pie4peu4pWh4q684pyC4q2J4pqn4oO24quV4pms4ruS4oet4qCl4o+i4rOh4oW94oac4o+B4rCC4q+"
			"F4pKl4pSS4pGE4oek4o6h4rex4q2O4oGT4oO/"
			"4puN4oC84ry74pin4oSb4o6m4qea4qKx4ryz4oul4ruu4o2p4pud4r6y4oe14rKq4o6k4rO74rSN4qOG4oi44oa3"),
		Entry(
			"⡷≗⯚⚲₷⇊⾂⠆⋀∡ⴼ⠧ ⚺⠸❃┌ⰕⰝ⁂⻕⃎⇥⠩☴❪⸗⽿▉➳⚳␛♢₨⭋"
			"⟁"
			"❜"
			"⤨"
			"⨭"
			"⩂"
			"❻"
			"⚳"
			"⬠"
			"⽦"
			"⧌"
			"➷",
			"4qG34omX4q+"
			"a4pqy4oK34oeK4r6C4qCG4ouA4oih4rS84qCn4oCp4pq64qC44p2D4pSM4rCV4rCd4oGC4ruV4oOO4oel4qCp4pi04p2q4riX4r2/"
			"4paJ4p6z4pqz4pCb4pmi4oKo4q2L4p+B4p2c4qSo4qit4qmC4p274pqz4qyg4r2m4qeM4p63")
	};
};

#endif
