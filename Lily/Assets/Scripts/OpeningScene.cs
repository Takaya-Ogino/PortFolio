using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.Video;

public class OpeningScene : MonoBehaviour {

    enum SceneState {
        Start,      // 開始時
        Play        // 再生時
    }
    SceneState scenestate = SceneState.Start;

    // ムービーを1度見ているかどうかを判定するフラグ
    public static bool isFirst = true;

    // コンポーネントを参照するための変数
    private VideoPlayer videoplayer;

    // Start is called before the first frame update
    void Start() {
        //　コンポーネント取得
        videoplayer = GetComponent<VideoPlayer>();

        videoplayer.isLooping = true;

        // ループ地点（ムービーの最後）に到達したらシーン遷移をします
        videoplayer.loopPointReached += LoadStageScene;
        StartCoroutine(OnStartMovie());
    }

    // ムービー開始時の処理
    IEnumerator OnStartMovie() {
        // 開始後１秒間は入力を受け付けないようにします
        yield return new WaitForSeconds(1);

        scenestate = SceneState.Play;
        yield return null;
    }
    // Update is called once per frame
    void Update() {
        switch (scenestate) {
            case SceneState.Play:
                UpdateForPlayState();
                break;
        }
    }

    // ムービー再生中の処理
    private void UpdateForPlayState() {
        // ２度目以降にSubmitボタンを押すとムービーをスキップできます
        if (Input.GetButtonDown("Submit")) {
            if (!isFirst) {
                SceneManager.LoadScene("Stage");
            }
        }
    }

    // Stageシーンに遷移します
    private void LoadStageScene(VideoPlayer videoPlayer) {
        // ムービーを停止してisFirstをfalseにします
        videoPlayer.Stop();
        isFirst = false;
        SceneManager.LoadScene("Stage");
    }
}