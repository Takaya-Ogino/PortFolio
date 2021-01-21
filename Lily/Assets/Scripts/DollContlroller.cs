using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//　敵キャラクター(人形)の動きを制御します
public class DollContlroller : MonoBehaviour {
    #region アニメーション用
    // 再生するアニメーションを指定します
    [SerializeField]
    private Object[] AnimationList;

    //再生するオーディオクリップを指定します
    [SerializeField]
    private AudioClip Attack;

    // アニメーションの種類を表します
    [SerializeField]
    enum AnimationPattern {
        Attack = 0,     // 攻撃
        Move = 1        // 移動
    }

    // 敵キャラクターのモーションの状態を表します
    enum MotionState {
        Start,      // ゲーム開始時
        Move,       // 移動
        Attack      // 攻撃
    }
    [SerializeField]
    MotionState motionState = MotionState.Start;

    // キャラクター位置座標のオフセットを指定します。
    [SerializeField]
    private Vector3 positionOffset = Vector3.zero;
    // キャラクターのスケールを指定します。
    [SerializeField]
    private Vector3 characterScale = Vector3.one;

    // SpriteStudio Anime を操作するためのクラス
    Script_SpriteStudio6_Root scriptRoot = null;
    #endregion

    // プレイヤーとの衝突判定用のコライダー
    [SerializeField]
    private BoxCollider2D playerChecker = null;

    // 敵キャラクターの移動速度を指定します
    [SerializeField]
    private float moveSpeed = 1.0f;

    // プレイヤー判定用の対象レイヤーを指定します
    [SerializeField]
    private LayerMask playerLayer;

    // プレイヤーのGameObject
    private GameObject player;

    // コンポーネント参照用の変数
    new Rigidbody2D rigidbody2D;
    AudioSource audiosuorce;

    // Start is called before the first frame update
    void Start() {
        // コンポーネント取得
        rigidbody2D = GetComponent<Rigidbody2D>();
        audiosuorce = GetComponent<AudioSource>();

        // プレイヤーをタグで検索
        player = GameObject.FindWithTag("Player");

        // アニメーション開始
        StartAnimation();
    }

    // Update is called once per frame
    void Update() {
        switch (motionState) {
            case MotionState.Start:
                UpdateForStartState();
                break;
            case MotionState.Move:
                UpdateForMoveState();
                break;
            case MotionState.Attack:
                break;
            default:
                break;
        }
    }

    // Moveステートに遷移します
    private void SetMoveState() {
        motionState = MotionState.Move;
        ChangeAnimation(AnimationPattern.Move);
    }

    // Attackステートに遷移します
    private void SetAttackState() {

        motionState = MotionState.Attack;
        ChangeAnimation(AnimationPattern.Attack);

        // 移動を止めます
        var velocity = rigidbody2D.velocity;
        velocity = Vector3.zero;
        rigidbody2D.velocity = velocity;

        // 攻撃の演出を開始します
        StartCoroutine(OnAttack());
    }

    // スタート時の処理
    private void UpdateForStartState() {
        // ゲーム開始時に移動を開始します
        SetMoveState();
    }

    // Moveステート時のフレーム更新処理
    private void UpdateForMoveState() {
        // 等速度運動
        var velocity = rigidbody2D.velocity;
        // プレイヤーに向かって指定した速度で移動します
        velocity = (player.transform.position - transform.position).normalized * moveSpeed;
        rigidbody2D.velocity = velocity;

        // 移動してる方向に対して向きを変更します
        if (velocity.x > 0) {
            transform.localScale = new Vector3(1, 1, 1);
        }
        else if (velocity.x < 0) {
            transform.localScale = new Vector3(-1, 1, 1);
        }
    }

    // プレイヤーに触れた際の演出処理
    IEnumerator OnAttack() {
        // 0.2秒待機してから攻撃音を再生する
        yield return new WaitForSeconds(0.2f);
        audiosuorce.PlayOneShot(Attack);

        yield return null;
    }

    // プレイヤーと接触したら攻撃モーションへ遷移します
    private void OnTriggerEnter2D(Collider2D collision) {
        // 侵入したトリガーのlayerを指定します
        var layerId = 1 << collision.gameObject.layer;

        // プレイヤーlayerに侵入したら攻撃処理に遷移します
        if ((layerId & playerLayer) != 0) {
            SetAttackState();
        }
    }

    #region アニメーション用関数
    // アニメーション開始 
    private void StartAnimation() {
        // 設定したプレハブアニメーションの数
        int listLength = AnimationList.Length;

        // すでにアニメーション生成済 or リソース設定無い場合はreturn
        if (scriptRoot != null || listLength < 1)
            return;

        // 再生するリソース名をリストから取得して再生する
        Object resourceObject = AnimationList[0];
        if (resourceObject != null) {
            // アニメーションを実体化
            var characterGameObject = Instantiate(resourceObject, Vector3.zero, Quaternion.identity) as GameObject;
            if (characterGameObject != null) {
                scriptRoot = Script_SpriteStudio6_Root.Parts.RootGet(characterGameObject);
                if (scriptRoot != null) {
                    // 座標設定するためのGameObject作成
                    var characterRoot = new GameObject("Doll");
                    if (characterRoot == null) {
                        // 作成できないケース対応 
                        Destroy(characterGameObject);
                        scriptRoot = null;
                    }
                    else {
                        // 座標設定 
                        characterGameObject.transform.SetParent(characterRoot.transform);

                        // 自分の子に移動して座標を設定
                        characterRoot.transform.SetParent(transform);
                        characterRoot.transform.localPosition = positionOffset;
                        characterRoot.transform.localRotation = Quaternion.identity;
                        characterRoot.transform.localScale = characterScale;

                        //アニメーション再生
                        ChangeAnimation(AnimationPattern.Move);
                    }
                }
            }
        }
    }

    // アニメーション 再生/変更 
    private void ChangeAnimation(AnimationPattern pattern) {
        // アニメーションが生成されていなければreturn
        if (scriptRoot == null) {
            return;
        }

        // 再生回数を指定します
        int timesPlay = 0;
        if (scriptRoot != null) {
            switch (pattern) {
                case AnimationPattern.Move:
                    timesPlay = 0;    // ループ再生 
                    break;
                case AnimationPattern.Attack:
                    timesPlay = 1;    // 1回だけ再生 
                    break;
                default:
                    break;
            }
            scriptRoot.AnimationPlay(-1, (int)pattern, timesPlay);
        }
    }
    #endregion
}
