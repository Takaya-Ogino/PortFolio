using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.Video;

// ゲームクリアー画面における進行制御を管理します。
public class GameClear : MonoBehaviour {
    // シーン遷移用のアニメーターを指定します。
    [SerializeField]
    private Animator sceneAnimator = null;

    // コンポーネント取得用の変数
    VideoPlayer videoplayer;

    // Start is called before the first frame update
    void Start() {
        // コンポーネント取得
        videoplayer = GetComponent<VideoPlayer>();
        videoplayer.isLooping = true;

        // ループポイント（ムービーの最後）に到達したらTitleシーンに遷移します
        videoplayer.loopPointReached += LoadTitleScene;
    }

    private void LoadTitleScene(VideoPlayer videoPlayer) {
        SceneManager.LoadScene("Title");
    }

}
