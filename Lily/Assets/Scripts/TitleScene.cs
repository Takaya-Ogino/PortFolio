using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;

// タイトル画面における進行制御を管理します。
public class TitleScene : MonoBehaviour {

    //タイトル画面上の"New Game", "Quit" の２つのボタンを指定します。
    [SerializeField]
    private Animator[] titleButtons = null;

    // 再生する音声を指定します（0:選択音　1:決定音）
    [SerializeField]
    private AudioClip[] clips;
    // 表示する操作説明画像を指定します
    [SerializeField]
    private GameObject manual;
    // 表示するクレジット画像を指定します
    [SerializeField]
    private GameObject credit;

    // 操作説明を開いているかどうかを判定するフラグ
    private bool isOpen;
    //現在選択されているボタンのID
    int titleButtonIndex = 0;

    // シーン遷移用のアニメーターを指定します。
    [SerializeField]
    private Animator sceneAnimator = null;
    // アニメーションのパラメーター
    static readonly int fadeOutTrigger = Animator.StringToHash("FadeOut Trigger");
    static readonly int isSelected = Animator.StringToHash("IsSelected");
    // コンポーネント参照用の変数
    AudioSource audiosource;

    // 前フレームの上下入力を記憶しておく変数
    float lastVertical = 0;

    // Start is called before the first frame update
    void Start() {
        // コンポーネント取得
        audiosource = GetComponent<AudioSource>();
        sceneAnimator = GetComponent<Animator>();

        isOpen = false;
        StartCoroutine(OnSelectButton());
    }


    private void Update() {
        // マニュアルを開いているときに決定ボタンを押すとマニュアルを閉じます
        if (isOpen) {
            if (Input.GetButtonDown("Submit")) {
                CloseManual();
            }
        }
    }

    // マニュアルを閉じてボタンの選択状態に戻ります
    private void CloseManual() {
        StartCoroutine(OnCloseManual());
    }

    IEnumerator OnCloseManual() {
        // マニュアルを非表示にします
        manual.SetActive(false);
        isOpen = false;

        // ボタンの選択状態に戻します
        yield return new WaitForSeconds(0.1f);
        StartCoroutine(OnSelectButton());
    }

    // ボタン選択中の状態
    IEnumerator OnSelectButton() {
        while (true) {
            //上下入力をしたら選択しているボタンを変更します
            var vertical = Input.GetAxisRaw("Vertical");
            // 今回初めて入力された場合
            if (lastVertical == 0) {
                if (vertical < 0) {
                    if (titleButtonIndex < titleButtons.Length - 1) {
                        // 選択音を再生します
                        audiosource.PlayOneShot(clips[0]);
                        titleButtonIndex++;
                    }
                }
                else if (vertical > 0) {
                    if (titleButtonIndex > 0) {
                        audiosource.PlayOneShot(clips[0]);
                        titleButtonIndex--;
                    }
                }
            }
            lastVertical = vertical;

            //決定ボタンが入力された場合
            if (Input.GetButtonDown("Submit")) {
                // 決定音を再生します
                audiosource.PlayOneShot(clips[1]);
                break;
            }
            //ボタンの選択状態を更新
            for (int index = 0; index < titleButtons.Length; index++) {
                //選択状態
                if (index == titleButtonIndex) {
                    titleButtons[index].SetBool(isSelected, true);
                }
                // 非選択状態
                else {
                    titleButtons[index].SetBool(isSelected, false);
                }
            }
            yield return null;
        }

        // 選択されたボタンに応じて処理をする
        switch (titleButtonIndex) {
            // New Game button
            case 0:
                // フェードアウトし、オープニングムービーを流します
                sceneAnimator.SetTrigger(fadeOutTrigger);
                yield return new WaitForSeconds(1);
                SceneManager.LoadScene("Opening");
                break;
            // Manual button
            case 1:
                // マニュアルを表示します
                manual.SetActive(true);
                isOpen = true;
                break;
            case 2:
                // Quit button
                // クレジットを表示します
                credit.SetActive(true);
                // 2秒間待機後ゲームを終了します
                yield return new WaitForSeconds(2f);
                Application.Quit();
                break;
        }
    }
}
