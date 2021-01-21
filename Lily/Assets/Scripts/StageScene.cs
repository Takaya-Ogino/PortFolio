using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

// ステージ画面における進行制御を管理します。
public class StageScene : MonoBehaviour {
    // フェードインアウト演出用の画像を指定します。
    [SerializeField]
    private Image transitionImage = null;

    // シーン遷移用のアニメーター
    Animator animator;
    // アニメーションのパラメーター
    static readonly int fadeOutTrigger = Animator.StringToHash("FadeOut Trigger");

    // ステージ数を指定します。
    [SerializeField]
    private int stageCount = 3;

    // 現在プレイしているステージ番号を取得または設定します。
    public static int StageNo {
        get { return stageNo; }
        set { stageNo = value; }
    }
    // 現在プレイしているステージ番号
    static int stageNo = 0;

    // ステージ画面内の進行状態を表します。
    enum SceneState {
        // ステージプレイ中
        Play,
        // ステージクリアーが確定していて演出中
        StageClear,
        // ゲームオーバーが確定していて演出中
        GemaOver
    }
    SceneState sceneState = SceneState.Play;

    #region インスタンスへのstaticなアクセスポイント
    // このクラスのインスタンスを取得します。
    public static StageScene Instance {
        get { return instance; }
        set { instance = value; }
    }
    private static StageScene instance = null;

    // AwakeメソッドはStartメソッドよりも先に実行したい初期化処理を記述します。
    void Awake() {
        // 生成されたインスタンス（自分自身）をstaticな変数に保存
        instance = this;

        // Resourcesフォルダーからステージのプレハブを読み込む
        var prefabName = string.Format("stage{0}", stageNo);
        var stagePrefab = Resources.Load<GameObject>(prefabName);
        Instantiate(stagePrefab, transform);
    }
    #endregion

    // Start is called before the first frame update
    void Start() {
        // コンポーネント取得
        animator = GetComponent<Animator>();

        // scenestateをPlayステートに設定します
        sceneState = 0;
    }

    #region ステージクリアー
    // このステージをクリアーさせます。
    public void StageClear() {
        // ステージプレイ中のみ
        if (sceneState == SceneState.Play) {
            sceneState = SceneState.StageClear;
            LoadNextScene();
        }
    }

    // 次のシーンを読み込みます。
    public void LoadNextScene() {
        StageNo++;
        // 残りのステージがまだ存在する場合
        if (StageNo < stageCount) {
            // 次のステージを読み込む
            LoadSceneWithFadeOut("Stage");
        }
        else {
            // ゲームクリアーの場合
            LoadSceneWithFadeOut("GameClear");
            StageNo = 0;
        }
    }
    #endregion

    #region ゲームオーバー
    // このステージをゲームオーバーで終了させます。
    public void GameOver() {
        // ステージプレイ中のみ
        if (sceneState == SceneState.Play) {
            sceneState = SceneState.GemaOver;
            LoadSceneWithFadeOut("GameOver");
        }
    }
    #endregion

    // フェイドアウト後に指定したシーンを読み込みます。
    public void LoadSceneWithFadeOut(string sceneName) {
        StartCoroutine(OnLoadSceneWithFadeOut(sceneName));
    }

    IEnumerator OnLoadSceneWithFadeOut(string sceneName) {
        // フェードアウト
        animator.SetTrigger(fadeOutTrigger);
        yield return new WaitForSeconds(1);
        SceneManager.LoadScene(sceneName);
    }
}
