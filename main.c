#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <emscripten.h>

// 文字列からアルファベットを除去
void remove_letters(char* dest, const char* src){
    int j = 0;
    for(int i=0; src[i]; i++){
        if(!isalpha(src[i])){  // アルファベットを除外
            dest[j++] = src[i];
        }
    }
    dest[j] = '\0';
}

void search(const char* target){

    FILE *fp = fopen("data.txt","r");

    if(!fp){
        EM_ASM({
            document.getElementById("result").innerText="ファイルが開けません";
        });
        return;
    }

    char line[256];
    char line_clean[256];

    // target もアルファベット除去して比較
    char target_clean[256];
    remove_letters(target_clean, target);

    while(fgets(line,sizeof(line),fp)){

        remove_letters(line_clean, line);

        if(strstr(line_clean,target_clean)){  // アルファベット無視で検索

            EM_ASM_({
                document.getElementById("result").innerText =
                    "結果: " + UTF8ToString($0);
            }, line);

            fclose(fp);
            return;
        }

    }

    fclose(fp);

    EM_ASM({
        document.getElementById("result").innerText="見つかりません";
    });
}

int main(){

    EM_ASM({

        let title = document.createElement("h2");
        title.innerText = "星位置検索";

        // カレンダー
        let date = document.createElement("input");
        date.type = "date";
        date.id = "date";

        // 文字入力
        let text = document.createElement("input");
        text.type = "text";
        text.placeholder = "例: 1991/01/03";
        text.id = "text";

        // ボタン
        let btn = document.createElement("button");
        btn.innerText = "検索";

        // 結果
        let result = document.createElement("div");
        result.id = "result";

        document.body.appendChild(title);
        document.body.appendChild(date);
        document.body.appendChild(document.createElement("br"));
        document.body.appendChild(text);
        document.body.appendChild(document.createElement("br"));
        document.body.appendChild(btn);
        document.body.appendChild(document.createElement("hr"));
        document.body.appendChild(result);

        btn.onclick = function(){

            let value = text.value;

            if(value==""){

                let d = date.value;

                if(d==""){
                    alert("日付入力してください");
                    return;
                }

                let p = d.split("-");

                // 日付だけ文字列化（YYYY/MM/DD）
                value = p[0] + "/" + p[1] + "/" + p[2];
            }

            ccall(
                "search",
                null,
                ["string"],
                [value]
            );
        }

    });

    return 0;
}