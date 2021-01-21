using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;

// ゲームオーバーUIのボタン操作などを管理します。
public class GameOver : MonoBehaviour {
    // ゲームオーバー画面の「Retry」ボタンと「Title」ボタンを指定します。
    [SerializeField]
    private Animator[] gameOverButtons = null;
    // 現在選択されているボタンID
    int gameOverButtonIndex = 0;

    //再生するオーディオクリップを指定します（0:選択音　1:決定音）
    [SerializeField]
    private AudioClip[] clips;

    // コンポーネント取得用の変数
    Animator sceneAnimator;
    AudioSource audiosource;

    // 前回フレームのX軸入力
    float lastHorizontal = 0;

    // アニメーションのパラメーター
    static readonly int fadeOutTrigger = Animator.StringToHash("FadeOut Trigger");
    static readonly int isSelected = Animator.StringToHash("IsSelected");

    // Start is called before the first frame update
    void Start() {
        // コンポーネント取得
        sceneAnimator = GetComponent<Animator>();
        audiosource = GetComponent<AudioSource>();

        StartCoroutine(OnGameOver());
    }

    // ゲームオーバーの演出を記述します。
    IEnumerator OnGameOver() {
        // ２秒待機
        yield return new WaitForSeconds(2);

        // ボタンの入力を待機
        while (true) {
            // 左右入力を取得
            var horizontal = Input.GetAxisRaw("Horizontal");
            // 今回初めて入力された場合
            if (lastHorizontal == 0) {
                if (horizontal > 0) {
                    if (gameOverButtonIndex < gameOverButtons.Length - 1) {
                        audiosource.PlayOneShot(clips[0]);
                        gameOverButtonIndex++;
                    }
                }
                else if (horizontal < 0) {
                    if (gameOverButtonIndex > 0) {
                        audiosource.PlayOneShot(clips[0]);
                        gameOverButtonIndex--;
                    }
                }
            }
            lastHorizontal = horizontal;
            // 決定ボタンが入力された場合
            if (Input.GetButtonDown("Submit")) {
                audiosource.PlayOneShot(clips[1]);
                break;
            }
            // ボタンの選択状態を更新
            for (int index = 0; index < gameOverButtons.Length; index++) {
                // 選択状態
                if (index == gameOverButtonIndex) {
                    gameOverButtons[index].SetBool(isSelected, true);
                }
                // 非選択状態
                else {
                    gameOverButtons[index].SetBool(isSelected, false);
                }
            }
            yield return null;
        }
        // 選択されているボタンに応じた処理をします
        switch (gameOverButtonIndex) {
            // Retry Button
            case 0:
                sceneAnimator.SetTrigger(fadeOutTrigger);
                yield return new WaitForSeconds(1);
                SceneManager.LoadScene("Stage");
                break;
            // Title Button
            case 1:
                sceneAnimator.SetTrigger(fadeOutTrigger);
                yield return new WaitForSeconds(1);
                // ステージナンバーを0に戻す
                StageScene.StageNo = 0;
                SceneManager.LoadScene("Title");
                break;
        }
    }
}
