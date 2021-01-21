using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 3ステージ目最後の蜘蛛型モンスターの行動を制御します
public class HahakuneController : MonoBehaviour {
    #region アニメーション用変数/ 列挙体
    // 再生するアニメーションを指定します
    [SerializeField]
    private Object[] AnimationList;

    // アニメーションの種類を表します
    [SerializeField]
    enum AnimationPattern : int {
        Menace = 0,     // 叫び（ゲームクリア時）
        Wait = 1        // 待機
    }

    // プレイヤーのモーション状態を表します。
    enum MotionState {
        Wait,       // 叫び
        Menace      // 咆哮
    }
    [SerializeField]
    MotionState motionState = MotionState.Wait;

    // キャラクター位置座標のオフセットを指定します。
    [SerializeField]
    private Vector3 positionOffset = Vector3.zero;
    // キャラクターのスケールを指定します。
    [SerializeField]
    private Vector3 characterScale = Vector3.one;

    // SpriteStudio Anime を操作するためのクラス
    Script_SpriteStudio6_Root scriptRoot = null;
    #endregion

    // プレイヤーの対象レイヤーを指定します
    [SerializeField]
    private LayerMask playerLayer;

    // 再生する音声を指定します(0:待機音　1:叫び声)
    [SerializeField]
    private AudioClip[] clips;
    // コンポーネント参照用の変数
    [SerializeField]
    private AudioSource audiosource;

    // Start is called before the first frame update
    void Start() {
        // コンポーネント取得
        audiosource = GetComponent<AudioSource>();

        // 待機音を再生します
        audiosource.clip = clips[0];
        audiosource.Play();

        // アニメーション開始
        AnimationStart();
    }

    // Menaceステートへ遷移します
    private void SetMenaceState() {
        // 待機音を停止してMenace処理へ移行します
        audiosource.Stop();
        StartCoroutine(OnMenacing());
    }

    // Menaceステート（ゲームクリア）演出の記述
    IEnumerator OnMenacing() {
        ChangeAnimation(AnimationPattern.Menace);
        
        // 1秒待機して叫び声を再生します
        yield return new WaitForSeconds(1f);
        audiosource.PlayOneShot(clips[1]);

        yield return null;
    }

    private void OnTriggerEnter2D(Collider2D collision) {
        var layerId = 1 << collision.gameObject.layer;

        // プレイヤーレイヤーに触れたらMenaceステートに遷移します
        if ((layerId & playerLayer) != 0) {
            SetMenaceState();
        }
    }

    #region アニメーション用関数
    // アニメーション開始 
    private void AnimationStart() {
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
                    var characterRoot = new GameObject("Hahakune");
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
                        ChangeAnimation(AnimationPattern.Wait);
                    }
                }
            }
        }
    }

    // アニメーション 再生/変更 
    private void ChangeAnimation(AnimationPattern pattern) {
        if (scriptRoot == null) {
            return;
        }

        // 再生する回数を指定します
        int timesPlay = 0;
        // 再生開始時のframeoffset（今回は0で固定）
        int frameOffset = 0;
        // アニメーションの再生速度を指定します
        float rateTime = 1;
        if (scriptRoot != null) {
            switch (pattern) {
                case AnimationPattern.Menace:
                    timesPlay = 1;
                    break;
                case AnimationPattern.Wait:
                    timesPlay = 1;
                    break;
                default:
                    break;
            }
            scriptRoot.AnimationPlay(-1, (int)pattern, timesPlay, frameOffset, rateTime);
        }
    }

    // アニメーションが再生中か停止中(エラー含)か取得します
    private bool IsAnimationPlay() {
        if (scriptRoot != null) {
            // 再生回数を取得して、プレイ終了かを判断します
            var remain = scriptRoot.PlayTimesGetRemain(0);
            if (remain >= 0) {
                return true;
            }
        }
        return false;
    }
    #endregion

}
